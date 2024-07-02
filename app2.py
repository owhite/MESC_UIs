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
import io
import logging
import matplotlib

matplotlib.use('agg')  # Set the backend to 'agg'

import matplotlib.pyplot as plt

import GoogleHandler
import HostMessages

from flask import Flask, render_template, request, jsonify, send_file

class MyFlaskApp:
    def __init__(self, config_file="config.ini"):
        self.app = Flask(__name__)
        self.upload_status_str = "Thread not started"
        self._setup_routes()
        self._setup_logging()
        self.task_queue = queue.Queue()

        self.image_visible = False

        self.banner_text = "Banner text"

        # Start the worker thread that handles non-GUI tasks
        self.worker_thread = threading.Thread(target=self.worker)
        self.worker_thread.start()

        # Start the worker thread that handles non-GUI tasks
        self.worker_thread = threading.Thread(target=self.worker)
        self.worker_thread.start()

        # house keeping
        config = configparser.ConfigParser()
        config.read(config_file)
        file_config = config['FILES']

        self.working_directory = os.path.join(os.path.expanduser("~"), ".log_UI")
        if not os.path.exists(self.working_directory):
            os.makedirs(self.working_directory)
        self.output_data_file = os.path.join(self.working_directory, file_config.get('logdata_file', 'MESC_logdata.txt'))
        self.output_plot_file = os.path.join(self.working_directory, file_config.get('plotdata_file', 'MESC_plt.png'))

        self.update_stats_running = True
        self.update_stats_thread = threading.Thread(target=self.updateStats)
        self.update_stats_thread.start()

        self.button_states = { 'b1': False }

        self.fig = None
        self.host = None

        if os.environ.get('WERKZEUG_RUN_MAIN') == 'true':
            # system messages and serial messages
            self.msgs = HostMessages.LogHandler(self)
            self.msgs.setDataLogFile(self.output_data_file)
            self.msgs.setPlotFile(self.output_plot_file)

            self.portName = None

            self.statusText = ''
            self.blink = True

            # collect up some things
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

            # manage the uploader
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

            # connect to the host's stream
            self.first_host_check = True
            self.host_log_stream = io.StringIO()
            stream_handler2 = logging.StreamHandler(self.host_log_stream)
            self.msgs.logger.addHandler(stream_handler2)

            # connect to the serial stream
            self.serial_log_stream = io.StringIO()
            stream_handler1 = logging.StreamHandler(self.serial_log_stream)
            self.msgs.serial_msgs.addHandler(stream_handler1)

            self.msgs.logger.info("GoogleHandler initiated")
            if self.drive.test_connection(): 
                self.msgs.logger.info("Ping to google drive working")


    def worker(self):
        while True:
            task = self.task_queue.get()
            if task is None:
                break
            func, args = task
            try:
                func(*args)
            except Exception as e:
                print(f"Error processing task: {e}")

    def updateStats(self):
        while self.update_stats_running:
            time.sleep(.1)

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
            text += f"Google spreadsheet {self.spreadsheet_id}\n"
            text += f"Worksheet {self.worksheet_name}\n"

        if self.internet and self.internet.check_internet_connection(): 
            text += (f"Internet connected, wifi name: {self.getWifiName()}\n")
        else:
            text += "Internet not working\n"
            
        if self.portName:
            text += (f"Serial port name: {self.portName}\n")
        else:
            text += (f"No serial port name: {self.portName}\n")

        # self.msgs.logger.info(text)
        return text

    def upload_task(self):
        self.upload_status_str = "Upload started"
        time.sleep(5)
        self.upload_status_str = "Upload finished"

    def _setup_routes(self):
        @self.app.route('/')
        def index():
            return render_template('index.html')

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

        @self.app.route('/toggle_image', methods=['POST'])
        def toggle_image():
            self.image_visible = not self.image_visible
            print("TOGGLE", self.image_visible)
            return jsonify(image_visible=self.image_visible)

        @self.app.route('/get_image', methods=['GET'])
        def get_image():
            print("GET", self.image_visible)

            f = self.msgs.getPlotFile()
            if self.image_visible and f:
                return send_file(f, mimetype='image/jpg')
            else:
                return '', 204

        @self.app.route('/image_status', methods=['GET'])
        def image_status():
            return jsonify(image_visible=self.image_visible)

        @self.app.route('/update_banner', methods=['POST'])
        def update_banner():
            data = request.json
            new_text = data.get('new_text', '')
            self.banner_text = new_text
            return jsonify({'status': 'success'})

        @self.app.route('/upload_thread', methods=['POST'])
        def start_upload_thread():
            # Add the upload task to the worker thread's task queue
            self.addTaskToQueue(self.upload_task)
            return jsonify(status=self.upload_status_str)

        @self.app.route('/upload_status', methods=['GET'])
        def upload_status_route():
            return jsonify(status=self.upload_status_str)

        @self.app.route('/tab3_selected', methods=['POST'])
        def tab3_selected():
            print('Tab 3 selected!')
            new_content = self.checkStatus()
            new_content = new_content.replace('\n', '<br>')
            return jsonify({'status': 'success', 'content': new_content})

        @self.app.route('/get_serial_msgs', methods=['GET'])
        def get_serial_msgs():
            log_content = self.serial_log_stream.getvalue()
            if log_content:
                print("Serial Messages Stream:", log_content)
                
            self.serial_log_stream.truncate(0)  # Clear the content
            self.serial_log_stream.seek(0)  # Reset stream position
            return jsonify({'logs': log_content})

        @self.app.route('/get_host_msgs', methods=['GET'])
        def get_host_msgs():
            host_content = self.host_log_stream.getvalue()
            if host_content:
                print("HOST Messages Stream:", host_content)
                
            self.host_log_stream.truncate(0)  # Clear the content
            self.host_log_stream.seek(0)  # Reset stream position
            return jsonify({'logs': host_content})


        @self.app.route('/send_command', methods=['PUT'])
        def send_command():
            command = request.data.decode('utf-8')  # Decode incoming text data
            if command:
                self.msgs.sendToPort(command)
                return jsonify({'status': 'success', 'message': f"Command '{command}' received and processed."})
            else:
                return jsonify({'status': 'error', 'message': 'No command received.'})

    def addTaskToQueue(self, func, *args):
        self.task_queue.put((func, args))

    def run(self):
        self.app.run(debug=True)

    def stop(self):
        self.task_queue.put(None)  # Stop the worker thread
        self.worker_thread.join()

    def _setup_logging(self):
        class NoStatusFilter(logging.Filter):
            def filter(self, record):
                return 'GET /get_serial_msgs' not in record.getMessage() and 'GET /get_host_msgs' not in record.getMessage() and 'GET /upload_status' not in record.getMessage()

        log = logging.getLogger('werkzeug')
        log.addFilter(NoStatusFilter())


if __name__ == '__main__':
    app = MyFlaskApp()
    try:
        app.run()
    finally:
        app.stop()
