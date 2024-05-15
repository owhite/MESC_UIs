#!/usr/bin/env python3

import logging
import configparser
import math
import os
import glob
import subprocess
import sys
import time
import threading

import matplotlib

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

import paho.mqtt.client as mqtt

import GoogleHandler
import HostMessages

# STATES:
#  wifi connected
#  MESC connected (usb/can)
#  GPS connected (phone)
#  Logging [on/off]
#  Uploading

# COMMANDS
#  get [wifi | mesc | gps | log | upload | all]
#  set [note | wifi | mesc | gps | log | upload]
#  cmd -- pass a command through to mesc
#  report - send a bundle of info on status
#  restart
#  quit

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

        print(self.output_data_file)

        # system messages and serial messages
        self.msgs = HostMessages.LogHandler(self)

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
    
        self.internet = GoogleHandler.PingInternet(self.msgs.logger)

        # self.msgs.openPort(self.portName)

        # manage the uploader
        google_config = config['GOOGLE']
        self.account_file = google_config.get('google_service_account', '')

        # Use a pre-existing spreadsheet, here
        # https://docs.google.com/spreadsheets/d/1iq2C9IOtOwm_KK67lcoUs2NjVRozEYd-shNs9lL559c/
        self.spreadsheet_id = google_config.get('spreadsheet_id', '1iq2C9IOtOwm_KK67lcoUs2NjVRozEYd-shNs9lL559c')
        self.worksheet_name = google_config.get('worksheeet_name', 'MESC_UPLOADS')
        print(self.msgs.logger, self.account_file, self.spreadsheet_id, self.worksheet_name)

        self.drive = GoogleHandler.handler(self.msgs.logger,
                                           self.account_file,
                                           self.spreadsheet_id,
                                           self.worksheet_name)

        self.msgs.logger.info("GoogleHandler initiated")
        if self.drive.test_connection(): 
            self.msgs.logger.info("google drive ping is working")

        self.statusText = ''
        self.timer = threading.Timer(0.1, self.updateStats)
        self.timer.start()

        self.log_is_on = False
        self.upload_thread = None

        mqtt_config = config['MQTT']

        client = mqtt.Client()
        client.on_connect = self.on_connect
        client.on_message = self.mqttEventHandler

        # Set username and password (if required by the MQTT broker)
        client.username_pw_set(mqtt_config.get('username', ''), mqtt_config.get('password', ''))

        # Connect to the MQTT broker
        try:
            client.connect(mqtt_config.get('broker', ''), mqtt_config.getint('port', ''), 60)
            client.loop_forever()
        except ConnectionRefusedError:
            self.msgs.logger.info("MQTT connection refused")
        except Exception as e:
            self.msgs.logger.info(F"Error connecting to MQTT broker {e}")

    # Define callback functions for MQTT events
    def on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT broker")
            # Subscribe to the topic of interest
            client.subscribe("ab")
        else:
            print("Failed to connect to MQTT broker with error code", rc)

    def mqttEventHandler(self, client, userdata, message):
        msg = message.payload.decode()
        self.msgs.logger.info(f"MQTT message: {msg} topic {message.topic}")

        if ' ' in msg:
            func, args = msg.split(' ', 1)
        else:
            func = msg
            args = None
        try:
            function_call = getattr(sys.modules[__name__], func)
            function_call(args)
        except AttributeError:
            self.msgs.logger.info(F"requested function: {func} not found")

    def updateStats(self):
        text =  "UPDATE STATS\n"
        if self.msgs:
            text += "message handler is not none\n"
        else:
            text += "message handler is none (that's bad)\n"
            
        if self.drive and self.drive.test_connection(): 
            text += "google drive connected\n"
        else:
            text += "google drive ping not working\n"

        self.spreadsheet_id
        self.worksheet_name

        if self.spreadsheet_id and self.worksheet_name:
            text += F"google spreadsheet {self.spreadsheet_id}\n"
            text += F"worksheet {self.worksheet_name}\n"

        if self.internet and self.internet.check_internet_connection(): 
            text += "internet connected\n"
        else:
            text += "internet not working\n"
            
        text += (F"internet name: {self.get_wifi_name()}\n")

        if self.portName:
            text += (F"serial port name: {self.portName}\n")
        else:
            text += (F"no serial port name: {self.portName}\n")

        self.statusText = text

    def get_wifi_name(self):
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

if __name__ == '__main__':
    TopApplication(config_file = "config.ini")

    try:
        while True:
            # perform other operations here
            pass
    except KeyboardInterrupt:
        print("Exiting...")
        subscriber.stop()
        print(f"Collected messages: {subscriber.myMessages}")

