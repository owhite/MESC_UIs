#! /usr/bin/env python3

# this handles two forms of logging:
#  1) it opens up system logs that sit on the host computer
#  2) it also handles various serial operations like opening/closing the port

import logging
import re
import sys
from datetime import datetime

from PyQt5 import QtCore
from PyQt5.QtCore import QTimer
from PyQt5.QtSerialPort import QSerialPort, QSerialPortInfo
from PyQt5.QtWidgets import QLabel

import Payload
import plotMESC


class LogHandler():
    def __init__(self, parent):
        self.parent = parent

        self.port = QSerialPort()
        self.serialPayload = Payload.Payload()
        self.serialPayload.startTimer()

        self.serial = None

        # serial_msgs handles sending the serial data that is not sent to disk, but goes to the UI
        self.serial_msgs = logging.getLogger('serial_msgs')
        self.serial_msgs.setLevel(logging.INFO)
        # data_logger handles sending serial data to disk, gets created later
        self.data_logger = None

        # dont confuse the service log, which just gets information from the program
        #  and sends it to serial_service.log, with the collection that is done by 
        #  initDataLogging() which actually collects data from the controller
        self.service_log_file = 'serial_service.log'
        self.logger = logging.getLogger('serial_service_log')
        self.logger.setLevel(logging.INFO)
        serial_service_handler = logging.FileHandler(self.service_log_file)
        stdout_handler = logging.StreamHandler(sys.stdout)

        formatter = logging.Formatter('%(asctime)s [%(levelname)s] %(message)s')
        serial_service_handler.setFormatter(formatter)
        stdout_handler.setFormatter(formatter)
        self.logger.addHandler(serial_service_handler)
        self.logger.addHandler(stdout_handler)

        self.term_collect_str = ''
        self.term_collect_flag = False

        self.json_collect_str = ''
        self.json_collect_flag = False
        self.hostStatusText = None


    def initHostStatusLabel(self, label):
        if isinstance(label, QLabel):
            self.hostStatusText = label

    def setStatusText(self, text):
        if self.hostStatusText and isinstance(self.hostStatusText, QLabel):
            self.hostStatusText.setText(text)

    def closePort(self):
        if self.serial:
            self.serial.close()

        
    def openPort(self, name):
        self.portName = name
        if not self.port.isOpen():
            self.port.setBaudRate( 115200 ) 
            self.port.setPortName( self.portName )
            self.port.setDataBits( 8 )
            self.port.setParity( 0 ) 
            self.port.setStopBits( 0 ) 
            self.port.setFlowControl( 0 ) 
            r = self.port.open(QtCore.QIODevice.ReadWrite)
            if not r:
                self.logger.info(F"Log Handler port: {self.portName} not open")
                self.setStatusText('Port open: error')
            else:
                self.logger.info(F"Log Handler opened for port: {self.portName}")
                self.setStatusText('Port opened')
                self.port.readyRead.connect(self.parseStream)
                self.sendToPort('su -g') # totally safe
                self.sendToPort('status stop')

        else:
            self.port.close()
            self.setStatusText('Port closed')

    def sendToPort(self, text):
        if not self.port.isOpen():
            self.logger.info("cant send cmd: {0} port not open".format(text))
        else:
            self.logger.info("sending cmd: {0}".format(text))
            text = text + '\r\n'
            self.port.write( text.encode() )

    def chow_block(self, text, token):
        flag = False

        t = text
        # not really intuitive but remove the token if at beginning of string
        #  the idea is the remainder will get save for the next round of parsing
        if text.startswith(token):
            t = text.replace(token, '', 1)

        if len(t) == 0: # you get this if string was empty or was just a prompt
            return False, t, t

        l = t.split(token)
        payload = ''
        remainder = ''
        if len(l) == 1: # no token, everything goes in remainder
            remainder = l[0]
        elif len(l) == 2:
            payload = l[0]
        elif len(l) > 2:
            flag = True
            payload = l[0]
            remainder = token.join(l[1:])
        else:
            print("chow_block: unforseen parsing condition")
            payload = "chow_block: unforseen parsing condition"

        # fakes the user out by showing the prompt at the beginning of the payload
        if len(payload) > 0:
            payload = token + payload

        return flag, payload, remainder

    # treat this function as a state machine
    def parseStream(self):
        data = self.port.readAll().data().decode()

        # strip vt100 chars
        ansi_escape = re.compile(r'\x1b[^m]*m|\x1b[^m]*$|^\x1b[^m]*')
        data = ansi_escape.sub('', data)
        data = re.sub('\\| ', '\t', data)

        # get current buffer, add the data
        self.serialPayload.concatString(data)
        
        remaining_text = ''
        s = self.serialPayload.reportString()

        json_text = ''
        if re.search("{.*}", s):
            s = s.replace('\r', '')
            s = s.replace("\x00", '')
            pattern = r'\{(.*?)\}\n'
            pattern = r'(\{.*\})\n'
            matches = re.findall(pattern, s, re.DOTALL)
            remaining_text = re.sub(pattern, '', s)
            self.serialPayload.setString(remaining_text)
            json_text = ''.join(matches)
            json_text = json_text.replace('\r', '').replace('\n', '')
            json_text = json_text.replace("}{", "}\n{") 

        if len(json_text) > 0:
            # split json to go to self.data_logger or self.serial_msgs
            if self.data_logger:
                if self.json_collect_flag:
                    self.data_logger.info("[JSON BLOCK]")
                    self.json_collect_flag = False
                self.data_logger.info(f"{json_text}")
            else:
                self.serial_msgs.info(f"{json_text}")

            self.serialPayload.resetTimer() 

        s = self.serialPayload.reportString()
        token = "usb@MESC>"

        result, first_block, second_block = self.chow_block(s, token)
        while(result):    
            print("BLOCK 1: " + first_block)
            result, first_block, second_block = self.chow_block(second_block, token)

        if len(first_block)!= 0:
            print ("BLOCK 2: " + first_block)

        self.serialPayload.setString(second_block)

    def initDataLogging(self, file_name):
        self.logger.info("Initiate data logging: %s")

        self.datafile_name = file_name
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
        file_handler = logging.FileHandler(self.datafile_name, mode="w") 
        file_handler.setFormatter(formatter)

        self.data_logger.addHandler(file_handler)
        self.logger.info(f"Created new output file log: {self.datafile_name}")

        self.term_collect_str = ''

        self.term_collect_flag = True
        self.json_collect_flag = True

        self.sendToPort('get')
        self.serialPayload.resetTimer() 
        self.sendToPort('status json')


    def endDataLogging(self, plot_file):
        self.sendToPort('status stop')
        self.plot_file = plot_file
        self.term_collect_flag = False
        self.json_collect_flag = False

        if self.data_logger:
            for handler in self.data_logger.handlers[:]:
                self.data_logger.removeHandler(handler)
                handler.close()
            self.data_logger = None
            self.logger.info("Finished logging")
            self.setStatusText("Finished logging")

            datatypes = ('ehz', 'phaseA', 'iqreq', 'vbus')
            p = plotMESC.PlotMESCOutput()
            (plt, fig) = p.generatePlot(self.datafile_name, datatypes)
            if plt is not None and fig is not None:
                self.logger.info(F"Created plot: {self.plot_file}")
                self.setStatusText("Created plot file")
                plt.savefig(self.plot_file)
            else:
                self.setStatusText("No plot saved")
                self.logger.info(F"No plot saved")

def main():
    pass

if __name__ == "__main__":
    main()

