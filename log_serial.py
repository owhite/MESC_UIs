#! /usr/bin/env python3

# this was a test application that sits on an RPI, opens serial connections to both the MESC controller
#   and a user's android. User is able to send commands to the on their phone to the MESC contoller, 
#   and this app handles logging data from the MESC. 
# Mostly used for testing, it is still incomplete, does not upload data to anywhere. 

import serial
import logging
import threading
import Payload
import time
import sys
import re
from datetime import datetime

class SerialHandler:
    def __init__(self, port, name):
        self.port = port
        self.name = name
        self.serial = None

        self.data_handler = None
        self.data_logger = None

        self.serialPayload = Payload.Payload()
        self.serialPayload.startTimer()

        self.service_log_file = 'serial_service.log'
        self.serial_log_file = 'output.log'

        self.output_file = ''

        self.logger = logging.getLogger('serial_service_log')
        self.logger.setLevel(logging.INFO)
        serial_service_handler = logging.FileHandler(self.service_log_file)
        stdout_handler = logging.StreamHandler(sys.stdout)
        formatter = logging.Formatter('%(asctime)s [%(levelname)s] %(message)s')
        serial_service_handler.setFormatter(formatter)
        stdout_handler.setFormatter(formatter)
        self.logger.addHandler(serial_service_handler)
        self.logger.addHandler(stdout_handler)

        self.logger.info("SerialTester initialized for port: %s", port)

        # self.drive = GoogleHandler.handler(self.logger)
        # self.logger.info("GoogleHandler initiated")

        self.term_collect_str = ''
        self.term_collect_flag = False

        self.json_collect_str = ''
        self.json_collect_flag = False

    def closePort(self):
        if self.serial:
            self.serial.close()

    def openPort(self):
        try:
            self.serial = serial.Serial(self.port, 115200)
            self.logger.info("SerialHandler opened for port: %s", self.port)
        except serial.SerialException as e:
            self.logger.error("Error opening the serial port: %s", e)

    def sendToPort(self, text):
        self.logger.info("sending cmd: {0}".format(text))
        text = text + '\r\n'
        self.serialPayload.resetString()
        self.serial.write( text.encode() )

    def serialReader(self):
        try:
            while True:
                if self.serial and self.serial.in_waiting > 0:
                    data = self.serial.read(self.serial.in_waiting).decode()
                    self.parseStream(data)
        except serial.SerialException as e:
            self.logger.error("Error opening the serial port: %s", e)
            # Handle the exception as needed
        
    def parseStream(self, data):
        # strip vt100 chars
        ansi_escape = re.compile(r'\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])')
        data = ansi_escape.sub('', data)
        data = re.sub('\\| ', '\t', data)

        # get current buffer, add the data
        r = self.serialPayload.reportString() + data
        
        # Extract json strings from input buffer
        text_inside_braces = ''
        pattern = r'(\{[^}]+\}\r\n)'            # find text between "{.*}\r\n"
        matches = re.findall(pattern, r)        # get all matches
        text_inside_braces = ''.join(matches)   # concatenate all matches
        remaining_text = re.sub(pattern, '', r) # remove these
        pattern = r'(\r)'                       # more purification
        remaining_text = re.sub(pattern, '', remaining_text)

        if len(text_inside_braces) > 0:
            t = text_inside_braces.replace('\r', '').replace('\n', '')
            if self.data_logger:
                # t = text_inside_braces.replace('\r', '').replace('\n', '')
                if self.json_collect_flag:
                    self.data_logger.info("[JSON BLOCK]")
                    self.json_collect_flag = False
                self.data_logger.info(f"{t}")
            else:
                print(f"{t}")

            self.serialPayload.resetTimer() 

        # hoping this means we have a complete block from terminal
        if remaining_text.endswith("@MESC>"):
            s = self.serialPayload.reportString()
            cmd = s.split("\n")
            if self.data_logger:
                if len(cmd) > 2:
                    self.data_logger.info("[{0}]\n{1}".format(cmd[0],"\n".join(cmd[1:])))
                else:
                    self.data_logger.info("[{0}]\n{1}".format(cmd[0],self.serialPayload.reportString()))
            else:
                print(F"PAYLOAD: {self.serialPayload.reportString()}")
            self.serialPayload.resetString()
            r = ''
            data = ''
            remaining_text = ''
            
        self.serialPayload.setString(remaining_text)

    def initDataLogging(self, cmd):
        self.logger.info("Initiate data logging: %s")

        c = cmd.split()
        today = datetime.today()
        current_time = datetime.now()
        military_time = current_time.strftime("%H:%M")
        formatted_date = today.strftime("%m-%d-%y")

        self.output_file = self.serial_log_file + "_" + formatted_date + "_" + military_time + ".txt"

        if len(c) == 2:
            self.output_file = c[1] + "-" + formatted_date + "_" + military_time + ".txt"

        if self.data_logger:
            for handler in self.data_logger.handlers[:]:
                self.data_logger.removeHandler(handler)
                print("Deleted logger 'output_log'.")
                handler.close()
            self.data_logger = None

        # Create a new logger for output.log (overwriting the file each time)
        self.data_logger = logging.getLogger('output_log')
        self.data_logger.setLevel(logging.INFO)

        formatter = logging.Formatter('%(message)s')
        file_handler = logging.FileHandler(self.output_file, mode="w") 
        file_handler.setFormatter(formatter)

        self.data_logger.addHandler(file_handler)
        self.logger.info(f"Created new output file log: {self.output_file}")

        self.term_collect_str = ''

        self.term_collect_flag = True
        self.json_collect_flag = True

        self.sendToPort('get')
        self.sendToPort('status json') 

    def endDataLogging(self):
        self.sendToPort('status stop')

        self.term_collect_flag = False
        self.json_collect_flag = False

        if self.data_logger:
            for handler in self.data_logger.handlers[:]:
                self.data_logger.removeHandler(handler)
                handler.close()
            self.data_logger = None
            self.logger.info("Finished logging")

    def uploadToDrive(self):
        dest_name = 'thing.txt'

        if self.output_file == '':
            self.logger.info("FAILED output file save, no file recently made")
            return()

        file_id, file_url =  self.drive.upload_file(self.output_file, dest_name)

        # List folders and files
        l = self.drive.list_files()

        for item in l:
            self.logger.info(f"Created {item}")
            if item['name'] == dest_name:
                self.drive.delete_files(item['id'])

        file_id, file_url =  self.drive.upload_file(file_path, dest_name)
        self.drive.set_permissions(file_id)

def main():
    port_name = '/dev/tty.usbmodem3552356B32321'

    # this serial should be connecting from RPI to MESC
    ser = SerialHandler(port_name, "one")
    ser.openPort()

    # this serial should connects from RPI to android
    port_name = '/dev/cu.usbmodem3552356B32321'
    BTser = SerialHandler(port_name, "two")

    p = plot.PlotMESCOutput()
    oname = 'scrap'

    try:
        # the connection to MESC is threaded
        reader_thread = threading.Thread(target=ser.serialReader, daemon=True)
        ser.logger.info("Starting serial thread.")
        reader_thread.start()

        while True:
            user_input = input("Enter command: ")
            user_input = user_input.lower()
            if user_input:
                if user_input.lower() == 'quit':
                    break
                elif user_input.startswith('log'):
                    ser.initDataLogging(user_input)
                elif user_input.startswith('test'):
                    ser.test_logger()
                elif user_input.startswith('end'):
                    ser.endDataLogging()
                elif user_input.startswith('upload'):
                    ser.uploadToDrive()
                elif user_input.startswith('plot'):
                    datatypes = ('ehz', 'phaseA', 'iqreq', 'vbus')
                    (plt, fig) = p.generatePlot(dname, datatypes, dname.replace(".txt", ""))
                    plt.savefig(oname + ".png")
                    plt.close(fig)
                else:
                    ser.sendToPort(user_input)
            time.sleep(.2)

        ser.logger.info("Program interrupted, serial port closed, thread ending") # presumably

    except KeyboardInterrupt:
        ser.logger.info("Program interrupted, serial port closed, thread ending")

if __name__ == "__main__":
    main()
