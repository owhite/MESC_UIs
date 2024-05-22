#!/usr/bin/env python3

import configparser
import glob
import json
import os
import signal
import subprocess
import sys
import threading
import time

import paho.mqtt.client as mqtt

import GoogleHandler
import HostMessages

# STATES:
#  wifi connected
#  MESC connected (usb/can)
#  GPS connected (phone)
#  Logging [on/off]
#  Uploading

# ?? 
# phone client: paho328137719229106

class TopApplication():
    def __init__(self, config_file="config.ino"):

        config = configparser.ConfigParser()
        config.read(config_file)
        file_config = config['FILES']

        # house keeping
        self.working_directory = os.path.join(os.path.expanduser("~"), ".log_UI")
        if not os.path.exists(self.working_directory):
            os.makedirs(self.working_directory)
        self.output_data_file = os.path.join(self.working_directory, file_config.get('logdata_file', 'MESC_logdata.txt'))
        self.output_plot_file = os.path.join(self.working_directory, file_config.get('plotdata_file', 'MESC_plt.png'))

        # system messages and serial messages
        self.msgs = HostMessages.LogHandler(self)
        self.msgs.setDataLogFile(self.output_data_file)
        self.msgs.setDataLogFile(self.output_plot_file)

        self.portName = None

        # collect up some things
        if sys.platform.startswith('darwin'):
            self.msgs.logger.info("macOS detected")
            matches = glob.glob('/dev/tty.usbmodem*')
            if len(matches) == 1:
                self.portName = matches[0]
            # this is stupid AF, but...
        elif sys.platform.startswith('linux'):
            self.portName = '/dev/ttyACM0'
            self.msgs.logger.info("linux detected")
            print("RASPBERRY account file use: /home/pi/mesc-data-logging-083b86e157cf.json")
        else:
            self.msgs.logger.info("Unknown operating system")
            sys.exit()
    
        self.internet = GoogleHandler.Ping(self.msgs.logger)

        # self.msgs.openPort(self.portName)

        # manage the uploader
        google_config = config['GOOGLE']
        self.account_file = google_config.get('google_service_account', '')

        # Use a pre-existing spreadsheet, here
        # https://docs.google.com/spreadsheets/d/1iq2C9IOtOwm_KK67lcoUs2NjVRozEYd-shNs9lL559c/
        self.spreadsheet_id = google_config.get('spreadsheet_id', '1iq2C9IOtOwm_KK67lcoUs2NjVRozEYd-shNs9lL559c')
        self.worksheet_name = google_config.get('worksheeet_name', 'MESC_UPLOADS')

        self.msgs.logger.info(F"Google account {self.account_file}" )
        self.msgs.logger.info(F"Google spreadsheet {self.spreadsheet_id}" )
        self.msgs.logger.info(F"Google worksheet {self.worksheet_name}" )

        self.drive = GoogleHandler.handler(self.msgs.logger,
                                           self.account_file,
                                           self.spreadsheet_id,
                                           self.worksheet_name)

        self.msgs.logger.info("GoogleHandler initiated")
        if self.drive.test_connection(): 
            self.msgs.logger.info("Ping to google drive working")


        self.upload_thread = GoogleHandler.ThreadOperation(self.drive, self.msgs.logger)
        self.upload_thread.setDataLogFile(self.output_data_file)
        self.upload_thread.setPlotFile(self.output_plot_file)

        self.mqtt_heartbeat = time.time() - 20
        self.mqtt_stopped = True
        self.mqtt_config = config['MQTT']
        self.initMQTT()
        time.sleep(1)

        self.statusText = ''
        self.blink = True
        self.thread = threading.Thread(target=self.updateStats)
        self.updateStatsRunning = True
        self.thread.start()

    def initMQTT(self):
        self.client = mqtt.Client()
        self.client.on_connect = self.mqttConnect
        self.client.on_message = self.mqttEventHandler
        self.client.username_pw_set(self.mqtt_config.get('username', ''), self.mqtt_config.get('password', ''))
        self.connected = False
        self.subscribed = False
        self.broker_address = self.mqtt_config.get('broker', '')
        self.topic = self.mqtt_config.get('topic', '')

        def signal_handler(sig, frame):
            self.msgs.logger.info("MQTT disconnecting")
            self.client.disconnect()
            sys.exit(0)

        signal.signal(signal.SIGINT, signal_handler)

        try:
            self.client.connect(self.broker_address, self.mqtt_config.getint('port', ''), 60)
            self.client.loop_start()
        except ConnectionRefusedError:
            self.msgs.logger.info("MQTT connection refused")
        except Exception as e:
            self.msgs.logger.info(F"Error connecting to MQTT broker {e}")

    def mqttDisconnect(self):
        if self.client is not None:
            self.connected = False
            print('Disconnecting from MQTT broker...')
            self.client.disconnect()
            print('Disconnected.')

    def mqttConnect(self, client, userdata, flags, rc):
        if rc == 0:
            self.msgs.logger.info("Connected to MQTT broker")
            client.subscribe(self.mqtt_config.get('topic', ''))
            self.connected = True
            self.subscribed = True
        else:
            self.msgs.logger.info(F"Failed to connect to MQTT broker {rc}")

    def mqttMessage(self, topic, message, silent = False):
        m_trunc = message.replace('\n', '')
        if len(m_trunc) > 50:
            m_trunc = m_trunc[:50] + "...."
        if self.connected:
            self.client.publish(topic, message)
            if not silent:
                self.msgs.logger.info(f"Published message to {topic}: {m_trunc}")
        else:
            if not silent:
                self.msgs.logger.info("Cannot publish message. Not connected to MQTT broker.")

    # DESIGN NOTE:
    # these calls are the only place where the program has UI-specific function calls
    # there are also commands that are sent from config.ini
    # 
    # sets the banner of the mqtt app
    def setMQTTBanner(self, text, color, background_color):
        s = F"{{\"_type\":\"banner_set\", \"visible\": true, \"size\":40, \"background\":\"{background_color}\", \"color\":\"{color}\" ,\"text\":\"{text}\"}}"
        self.mqttMessage(self.topic, s)

    # change state of button
    def setMQTT_Button(self, num, state, silent = False):
        s = F"{{\"_type\": \"button_set\", \"number\": {num}, \"state\": false}}"
        if state:
            s = F"{{\"_type\": \"button_set\", \"number\": {num}, \"state\": true}}"
        self.mqttMessage(self.topic, s, silent)

    # change state of LED
    def setMQTT_LED(self, num, state, silent = False):
        s = F"{{\"_type\": \"LED_set\", \"number\": {num}, \"state\": false}}"
        if state:
            s = F"{{\"_type\": \"LED_set\", \"number\": {num}, \"state\": true}}"
        self.mqttMessage(self.topic, s, silent)

    def updateStats(self):
        while self.updateStatsRunning:
            if time.time() - self.mqtt_heartbeat < 20: # arbitrary time testing if we havent heard from app
                if self.mqtt_stopped:
                    self.initMQTTAppUI()
                    self.mqtt_stopped = False
                else:
                    if self.internet and self.internet.check_internet_connection(): 
                        # tell this to be quiet about it with True parameter
                        self.setMQTT_Button(4, True, True)
                    else:
                        self.setMQTT_Button(4, False, True)
                    self.setMQTT_Button(5, True, True)
                    self.setMQTT_LED(3, self.blink, True)
                    self.blink = not self.blink
            else:
                # if we're here, it is okay to send initializing commands, but commands involving "set" break the app
                self.mqtt_stopped = True
                self.initMQTTAppUI()
            time.sleep(1)  

    def initMQTTAppUI(self):
        # initialize LEDs
        self.mqttMessage(self.topic, self.mqtt_config.get('led_init', ''))
        # initialize buttons
        self.mqttMessage(self.topic, self.mqtt_config.get('button_init', ''))
        # set banner
        self.setMQTTBanner("MESC Logger", "FFFFFF", "8E44AD")
        if self.internet and self.internet.check_internet_connection(): 
            # tell this to be quiet about it with True parameter
            self.setMQTT_Button(4, True, True)
        else:
            self.setMQTT_Button(4, False, True)

    def getWifiName(self):
        if sys.platform.startswith('darwin'):
            # the world's most 'unlikely to survive over time'-idea:
            process = subprocess.Popen(['/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport','-I'], stdout=subprocess.PIPE)
            out, err = process.communicate()
            process.wait()

            network_info_str = out.decode('utf-8')
            lines = network_info_str.split('\n')

            # Iterate through each line
            d = {}
            for line in lines:
                line = line.lstrip()
                if ":" in line:
                    l = line.split(':')
                    d[l[0].lstrip()] = l[1].lstrip()

            if d.get('SSID'):
                if d['SSID'] == 'Off':
                    return None
                return d['SSID']
            else:
                return None
        elif sys.platform.startswith('linux'):
            try:
                result = subprocess.run(["iwgetid", "-r"], capture_output=True, text=True)
                if result.returncode == 0:
                    wifi_network_name = result.stdout.strip()
                    return wifi_network_name
                else:
                    return None
            except Exception as e:
                return None
        else:
            return None

    def mqttEventHandler(self, client, userdata, message):
        msg = message.payload.decode()
        msg = msg.rstrip()

        if msg.startswith('{'):
            self.parseJSON(msg)
            return
        else:
            if ' ' in msg:
                func, args = msg.split(' ', 1)
            else:
                func = msg 
                args = None

        # IMO a really fun technique. Test to see if a function exists,
        #   if it does, then call it. errors are handled rather nicely
        if hasattr(self, func) and callable(getattr(self, func)):
            try:
                self.msgs.logger.info(F"Incoming cmd: {{{func} {args}}}")
                if args is None:
                    getattr(self, func)(None)
                else:
                    getattr(self, func)(args)

            except Exception as e:
                self.msgs.logger.info(F"requested function {{{func}}} error: {{{e}}}")

    ###############################
    # These functions called by mqttEventHandler()
    #  parseJSON
    #  set [note]
    #  start [log | upload]
    #  stop  [log]
    #  check [log | status ] status, creates a text bundle of info
    #  quit
    # 
    # TO DO:
    #  cmd -- pass a command through to mesc
    #  get [wifi | port | files | things]
    #  restart [wifi | port | mqtt | things]

    def parseJSON(self, json_string):
        # right now this just parses things of the form:
        #   {"data":{"bearing":0.0,"deviceId":"c16","latitude":39.30,"longitude":-76.61,"speed":0.0,"timeStamp":"2024-05-19 13:14:45"},"type":"location"}
        try:
            dict = json.loads(json_string)
            if dict.get('type') and dict['type'] == 'location':
                if dict.get('data'):
                    self.mqtt_heartbeat = time.time() # needed to know the android app is connected
                    print (F"JSON: {dict['data']}")
            
        except json.JSONDecodeError as e:
            self.msgs.logger.info(F"Tried json parse, failed: {{{json_string}}} {e}")
            return False
        return True


    def start(self, args):
        if args is None:
            # restart the server? the ui? 
            return
        if args == 'log':
            if self.msgs.logIsOn():
                self.msgs.endDataLogging()
            self.msgs.initDataLogging()

        if args == 'upload':
            self.upload_thread.start()

    def stop(self, args):
        if args is None:
            return
        if args == 'log':
            if self.msgs.logIsOn():
                self.msgs.endDataLogging()
            self.msgs.initDataLogging()
        if args == 'upload':
            self.upload_thread.stop()

    def check(self, args):
        if args is None:
            return
        if args == 'log':
            if self.upload_thread.threadIsRunning():
                self.msgs.logger.info(F"log upload running")
            else:
                self.msgs.logger.info(F"log upload not running")
        if args == 'status':
            self.checkStatus()

    def checkStatus(self):
        text =  "----UPDATE STATS----\n"
        if self.msgs:
            text += "Message handler appears to be up\n"
        else:
            text += "Message handler appears to be down\n"
            
        if self.drive and self.drive.test_connection(): 
            text += "Ping to google working\n"
        else:
            text += "Ping to google not working\n"

        if self.spreadsheet_id and self.worksheet_name:
            text += F"Google spreadsheet {self.spreadsheet_id}\n"
            text += F"Worksheet {self.worksheet_name}\n"

        if self.internet and self.internet.check_internet_connection(): 
            text += (F"Internet connected, wifi name: {self.getWifiName()}\n")
        else:
            text += "Internet not working\n"
            
        if self.portName:
            text += (F"Serial port name: {self.portName}\n")
        else:
            text += (F"No serial port name: {self.portName}\n")

        if self.subscribed:
            text += (F"Connected to MQTT broker: {self.broker_address}\n")
        else:
            text += (F"Cant connect to MQTT broker: {self.broker_address}\n")

        text += (F"The state of mqtt_stopped is: {self.mqtt_stopped}\n")
        text += (F"The state of connected is is: {self.connected}\n")
        text +=  "-UPDATE STATS-"

        self.msgs.logger.info(text)


    def quit(self):
        self.msgs.logger.info(F"User quitting")
        self.client.disconnect()
        sys.exit(0)


if __name__ == '__main__':
    TopApplication(config_file = "config.ini")

    try:
        while True:
            pass
    except KeyboardInterrupt:
        TopApplication.quit()
