#! /usr/bin/env python3

# this handles two forms of logging:
#  1) it opens up system logs that sit on the host computer
#  2) it also handles various serial operations like opening/closing the port

import logging
import re
import sys
from datetime import datetime

from PyQt5 import QtCore
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
                self.logger.info("Log Handler port: %s not open", self.portName)
                self.setStatusText('Port open: error')
            else:
                self.logger.info("Log Handler opened for port: %s", self.portName)
                self.setStatusText('Port opened')
                self.port.readyRead.connect(self.parseStream)
        else:
            self.port.close()
            self.setStatusText('Port closed')

    def sendToPort(self, text):
        if not self.port.isOpen():
            self.logger.info("cant send cmd: {0} port not open".format(text))
        else:
            self.logger.info("sending cmd: {0}".format(text))
            text = text + '\r\n'
            self.serialPayload.resetString()
            self.port.write( text.encode() )

    def parseStream(self):
        data = self.port.readAll().data().decode()

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
            # here is where the stream gets split to either go to self.data_logger or self.serial_msgs
            if self.data_logger:
                # t = text_inside_braces.replace('\r', '').replace('\n', '')
                if self.json_collect_flag:
                    self.data_logger.info("[JSON BLOCK]")
                    self.json_collect_flag = False
                self.data_logger.info(f"{t}")
            else:
                self.serial_msgs.info(f"{t}")

            self.serialPayload.resetTimer() 

        # hoping this means we have a complete block from terminal
        if remaining_text.endswith("@MESC>"):
            s = self.serialPayload.reportString()
            cmd = s.split("\n")

            # also where the stream gets split
            if self.data_logger:
                if len(cmd) > 2:
                    self.data_logger.info("[{0}]\n{1}".format(cmd[0],"\n".join(cmd[1:])))
                else:
                    self.data_logger.info("[{0}]\n{1}".format(cmd[0],self.serialPayload.reportString()))
            else:
                t = ''
                for line in self.serialPayload.reportString().split('\n'):
                    if line.count('\t') == 4:
                        l = line.split('\t')
                        w = l[0]
                        t = t + w[:10] + '\t' + l[1] + '\n'
                    else:
                        t = t + line + '\n'

                self.serial_msgs.info(t)

            self.serialPayload.resetString()
            r = ''
            data = ''
            remaining_text = ''
            
        self.serialPayload.setString(remaining_text)

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

        self.sendToPort('status stop')

        self.sendToPort('get')
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

