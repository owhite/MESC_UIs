import json
import serial
import configparser
import glob
import os
import subprocess
import sys
import threading
import time
import queue

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
        self.task_queue = queue.Queue()

        self.banner_text = "Banner text"

        # Start the worker thread that handles non-GUI tasks
        self.worker_thread = threading.Thread(target=self.worker)
        self.worker_thread.start()

        self.upload_thread = None
        self.update_status_thread = threading.Thread(target=self.updateStatus)
        self.update_status_thread_running = True
        self.update_status_thread.start()

        # Housekeeping
        config = configparser.ConfigParser()
        config.read(config_file)
        file_config = config['FILES']

        self.working_directory = os.path.join(os.path.expanduser("~"), ".log_UI")
        if not os.path.exists(self.working_directory):
            os.makedirs(self.working_directory)
        self.output_data_file = os.path.join(self.working_directory, file_config.get('logdata_file', 'MESC_logdata.txt'))
        self.output_plot_file = os.path.join(self.working_directory, file_config.get('plotdata_file', 'MESC_plt.png'))

        if os.environ.get('WERKZEUG_RUN_MAIN') == 'true':
            # System messages and serial messages
            self.msgs = HostMessages.LogHandler(self)
            self.msgs.setDataLogFile(self.output_data_file)
            self.msgs.setPlotFile(self.output_plot_file)

            self.portName = None

            # Collect up some things
            if sys.platform.startswith('darwin'):
                self.msgs.logger.info("macOS detected")
                matches = glob.glob('/dev/tty.usbmodem*')
                if len(matches) == 1:
                    self.portName = matches[0]
            elif sys.platform.startswith('linux'):
                self.portName = '/dev/ttyACM0'
                self.msgs.logger.info("linux detected")
                print("RASPBERRY account file use: /home/pi/mesc-data-logging-083b86e157cf.json")
            else:
                self.msgs.logger.info("Unknown operating system")
                sys.exit()
    
            self.internet = GoogleHandler.Ping(self.msgs.logger)

            self.msgs.openPort(self.portName)

            # Manage the uploader
            google_config = config['GOOGLE']
            self.account_file = google_config.get('google_service_account', '')

            self.spreadsheet_id = google_config.get('spreadsheet_id', '1iq2C9IOtOwm_KK67lcoUs2NjVRozEYd-shNs9lL559c')
            self.worksheet_name = google_config.get('worksheeet_name', 'MESC_UPLOADS')

            self.msgs.logger.info(f"Google account {self.account_file}")
            self.msgs.logger.info(f"Google spreadsheet {self.spreadsheet_id}")
            self.msgs.logger.info(f"Google worksheet {self.worksheet_name}")

            self.drive = GoogleHandler.handler(self.msgs.logger,
                                               self.account_file,
                                               self.spreadsheet_id,
                                               self.worksheet_name)

            self.msgs.logger.info("GoogleHandler initiated")
            if self.drive.test_connection(): 
                self.msgs.logger.info("Ping to google drive working")

            self.statusText = ''
            self.blink = True

    def worker(self): # Handles tasks
        while True:
            task = self.task_queue.get()
            if task is None:
                break
            func, args = task
            func(*args)

    def addTaskToQueue(self, func, *args):
        self.task_queue.put((func, args))

    def updateStatus(self):
        while self.update_status_thread_running:
            time.sleep(.1)
            # Perform status updates here if needed

    def getWifiName(self):
        if sys.platform.startswith('darwin'):
            # The world's most 'unlikely to survive over time'-idea:
            process = subprocess.Popen(['/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport', '-I'], stdout=subprocess.PIPE)
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
        text = ''
        if self.msgs:
            text += "Message handler appears to be up\n"
        else:
            text += "Message handler appears to be down\n"
            
        if self.drive and self.drive.test_connection(): 
            text += "Ping to google working\n"
        else:
            text += "Ping to google not working\n"

        if self.spreadsheet_id and self.worksheet_name:
            text += f"Google spreadsheet {self.spreadsheet_id}\n"
            text += f"Worksheet {self.worksheet_name}\n"

        if self.internet and self.internet.check_internet_connection(): 
            text += f"Internet connected, wifi name: {self.getWifiName()}\n"
        else:
            text += "Internet not working\n"
            
        if self.portName:
            text += f"Serial port name: {self.portName}\n"
        else:
            text += "No serial port name\n"

        self.msgs.logger.info(text)
        return text

    def set_banner_text(self, new_text):
        self.banner_text = new_text

    def launchUploadThread(self):
        self.upload_thread = GoogleHandler.uploadThread(self,
                                                        [self.output_data_file,
                                                         self.output_plot_file],
                                                        self.start_position,
                                                        self.output_note)
        return self.upload_thread

    def setup_routes(self):
        @self.app.route('/')
        def index():
            return render_template('index.html')

        @self.app.route('/update_banner', methods=['POST'])
        def update_banner():
            data = request.json
            new_text = data.get('new_text', '')
            self.set_banner_text(new_text)
            return jsonify({'status': 'success'})

        @self.app.route('/button1_click', methods=['POST'])
        def button1_click():
            button_id = request.json['button_id']
            if self.button_states['b1']:
                self.button_states['b1'] = False
                self.addTaskToQueue(self.msgs.endDataLogging)
            else:
                self.button_states['b1'] = True
                self.addTaskToQueue(self.msgs.initDataLogging)

            print(f'Button 1 clicked! Button ID: {button_id}, state: {self.button_states["b1"]}')
            return jsonify({'status': 'success', 'button_id': button_id, 'b1': self.button_states['b1']})

        @self.app.route('/button2_click', methods=['POST'])
        def button2_click():
            button_id = request.json['button_id']
            self.button_states['b2'] = not self.button_states['b2']
            self.addTaskToQueue(self.some_other_task, "Button 2 Task", 5)
            print(f'Button 2 clicked! Button ID: {button_id}, state: {self.button_states["b2"]}')
            return jsonify({'status': 'success', 'button_id': button_id, 'b2': self.button_states['b2']})

        @self.app.route('/button3_click', methods=['POST'])
        def button3_click():
            button_id = request.json['button_id']
            if self.button_states['b3']:
                self.button_states['b3'] = False
                if self.upload_thread:
                    self.upload_thread.stop()
            else:
                self.button_states['b3'] = True
                if not self.output_data_file or self.output_data_file == '':
                    self.status_label.setText("No log file to upload")
                else:
                    self.start_position = (0.0, 0.0)
                    self.output_note = ''
                    self.upload_thread = self.launchUploadThread()
                    self.upload_thread.start()

            print(f'Button 3 clicked! Button ID: {button_id}, state: {self.button_states["b3"]}')
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

    def stop(self):
        self.update_status_thread_running = False
        self.task_queue.put(None)  # Stop the worker thread
        self.worker_thread.join()
        self.update_status_thread.join()

if __name__ == '__main__':
    app = MyFlaskApp(config_file="config.ini")
    try:
        app.run()
    finally:
        app.stop()
