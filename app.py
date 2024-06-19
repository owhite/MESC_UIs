import json
import serial
import configparser
import glob
import json
import os
import subprocess
import sys
import threading
import time

import GoogleHandler
import HostMessages


from flask import Flask, render_template, request, jsonify

class MyFlaskApp:
    def __init__(self, config_file="config.ini"):
        self.app = Flask(__name__)

        self.button_states = {
            'b1': False,
            'b2': False,
            'b3': False
        }

        self.setup_routes()

        # house keeping
        config = configparser.ConfigParser()
        config.read(config_file)
        file_config = config['FILES']

        self.working_directory = os.path.join(os.path.expanduser("~"), ".log_UI")
        if not os.path.exists(self.working_directory):
            os.makedirs(self.working_directory)
        self.output_data_file = os.path.join(self.working_directory, file_config.get('logdata_file', 'MESC_logdata.txt'))
        self.output_plot_file = os.path.join(self.working_directory, file_config.get('plotdata_file', 'MESC_plt.png'))

        # system messages and serial messages
        self.msgs = HostMessages.LogHandler(self)
        self.msgs.setDataLogFile(self.output_data_file)
        self.msgs.setPlotFile(self.output_plot_file)

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

        self.msgs.openPort(self.portName)

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
        self.statusText = ''
        self.blink = True
        self.updateStatsRunning = True
        # self.thread = threading.Thread(target=self.updateStats)
        # self.thread.start()

    def updateStats(self):
        while self.updateStatsRunning:
            # do things to check on status
            print("THING")
            time.sleep(.1)  

    def parse_serial_data(self):
        if self.serial_connection:
            try:
                data = self.serial_connection.readline().decode('utf-8').strip()
                # Example parsing logic, adjust as needed
                parsed_data = {"raw_data": data, "length": len(data)}
                return parsed_data
            except serial.SerialException as e:
                print(f"Error reading from serial connection: {e}")
                return None
        return None

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

    def checkStatus(self):
        text =  ''
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

        self.msgs.logger.info(text)
        return text

    def setup_routes(self):
        @self.app.route('/')
        def index():
            return render_template('index.html')

        @self.app.route('/button1_click', methods=['POST'])
        def button1_click():
            button_id = request.json['button_id']

            if self.button_states['b1']:
                self.msgs.endDataLogging()
                self.button_states['b1'] = False
            else:
                self.msgs.initDataLogging()
                self.button_states['b1'] = True

            print(f'Button 1 clicked! Button ID: {button_id}, state: {self.button_states['b1']}')
            return jsonify({'status': 'success', 'button_id': button_id, 'b1': self.button_states['b1']})

        @self.app.route('/button2_click', methods=['POST'])
        def button2_click():
            button_id = request.json['button_id']
            self.button_states['b2'] = not self.button_states['b2']  # Toggle state
            print(f'Button 2 clicked! Button ID: {button_id}, state: {self.button_states['b2']}')
            return jsonify({'status': 'success', 'button_id': button_id, 'b2': self.button_states['b2']})

        @self.app.route('/button3_click', methods=['POST'])
        def button3_click():
            button_id = request.json['button_id']
            self.button_states['b3'] = not self.button_states['b3']  # Toggle state
            print(f'Button 3 clicked! Button ID: {button_id}, state: {self.button_states['b3']}')
            return jsonify({'status': 'success', 'button_id': button_id, 'b3': self.button_states['b3']})

        @self.app.route('/check_button_status', methods=['GET'])
        def check_button_status():
            return jsonify(self.button_states)

        @self.app.route('/tab3_selected', methods=['POST'])
        def tab3_selected():
            print('Tab 3 selected!')
            new_content = self.checkStatus()
            new_content = new_content.replace('\n', '<br>')
            return jsonify({'status': 'success', 'content': new_content})

    def run(self):
        self.app.run(debug=True)

if __name__ == '__main__':
    app = MyFlaskApp(config_file = "config.ini")

    app.run()
