#!/usr/bin/env python3

import sys
import logging
import threading
import time
import re
import plotMESC
import time, math, colorsys
from datetime import datetime

import Payload
import GoogleHandler

from PyQt5 import QtCore
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QVBoxLayout, QLabel, QLineEdit, QTextEdit
from PyQt5.QtCore import Qt, QEvent, QTimer, pyqtSignal
from PyQt5.QtGui import QTextCursor
from PyQt5.QtSerialPort import QSerialPort, QSerialPortInfo

import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure


class Log_Handler():
    def __init__(self, parent):

        self.parent = parent

        self.port = QSerialPort()

        self.serial = None
        self.statusText = parent.status_label

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

        # self.drive = GoogleHandler.handler(self.logger)
        # self.logger.info("GoogleHandler initiated")

        self.term_collect_str = ''
        self.term_collect_flag = False

        self.json_collect_str = ''
        self.json_collect_flag = False

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
                self.statusText.setText('Port open: error')
            else:
                self.logger.info("Log Handler opened for port: %s", self.portName)
                self.statusText.setText('Port opened')
                self.port.readyRead.connect(self.parseStream)
        else:
            self.port.close()
            self.statusText.setText('Port closed')

    def sendToPort(self, text):
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
                # print(F"PAYLOAD: {self.serialPayload.reportString()}")
                t = ''
                for line in self.serialPayload.reportString().split('\n'):
                    if line.count('\t') == 4:
                        l = line.split('\t')
                        w = l[0]
                        t = t + w[:10] + '\t' + l[1] + '\n'
                    else:
                        t = t + line + '\n'
                        
                self.parent.open_second_window(t)

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

class CmdWindow(QWidget):
    closing = pyqtSignal()

    def __init__(self):
        super().__init__()

        self.setWindowTitle('Second Window')
        self.setGeometry(1, 1, 800, 440)

        layout = QVBoxLayout()

        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)
        layout.addWidget(self.text_edit)

        self.setLayout(layout)

    def set_text(self, text):
        self.text_edit.setPlainText(text)
        cursor = self.text_edit.textCursor()
        cursor.movePosition(QTextCursor.Start)
        self.text_edit.setTextCursor(cursor)

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Escape:
            self.close()
        if event.key() == Qt.Key_Return:
            self.close()

    def closeEvent(self, event):
        self.closing.emit()
        super().closeEvent(event)

class PlotWindow(QWidget):
    closing = pyqtSignal()

    def __init__(self, parent):
        super().__init__()
        self.parent = parent

        dname = self.parent.plot_file
        oname = 'scrap'

        self.setWindowTitle('Matplotlib Window')
        self.setGeometry(1, 1, 800, 440)

        layout = QVBoxLayout()

        oname = 'scrap'
        datatypes = ('ehz', 'phaseA', 'iqreq', 'vbus')
        p = plot.PlotMESCOutput()
        (plt, fig) = p.generatePlot(dname, datatypes, dname.replace(".txt", ""))
        plt.savefig(oname + ".png")

        self.canvas = FigureCanvas(fig)
        layout.addWidget(self.canvas)

        self.setLayout(layout)
        self.canvas.draw()

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Escape:
            self.close()

    def closeEvent(self, event):
        self.closing.emit()
        super().closeEvent(event)

class UI_Handler(QWidget):
    def __init__(self):
        super().__init__()

        self.normal_color = "#FDFEFE"
        self.normal_border = "#BDC3C7"
        self.highlight_color = "#85C1E9" 
        self.highlight_border = "#229954"
        self.current_index = 0;
        self.index_to_button = {};

        self.button_w = 800 - 20
        self.button_h = 80

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.checkStatus)
        self.timer.start(50)
        self.log_is_on = False

        self.logger = None

        self.init_ui()

    def init_ui(self):
        # Set window geometry
        self.setGeometry(1, 1, self.button_w + 20, 420)
        self.setWindowTitle('MESC logger')

        # Create the label to display button click information
        self.status_label = QLabel('MESC Logger', self)
        self.status_label.setAlignment(Qt.AlignCenter)
        font = self.status_label.font()
        font.setPointSize(40)
        self.status_label.setFont(font)

        # Create the four buttons with larger font size
        self.log_button = QPushButton('LOG', self)
        font = self.log_button.font()
        font.setPointSize(60)
        self.log_button.setFont(font)
        self.log_button.setFixedSize(self.button_w, self.button_h)
        self.log_button.clicked.connect(self.log_button_clicked)
        self.index_to_button[0] = self.log_button

        self.plot_button = QPushButton('PLOT', self)
        font = self.plot_button.font()
        font.setPointSize(60)
        self.plot_button.setFont(font)
        self.plot_button.setFixedSize(self.button_w, self.button_h)
        self.plot_button.clicked.connect(self.plot_button_clicked)
        self.index_to_button[1] = self.plot_button

        self.upload_button = QPushButton('UPLOAD', self)
        font = self.upload_button.font()
        font.setPointSize(60)
        self.upload_button.setFont(font)
        self.upload_button.setFixedSize(self.button_w, self.button_h)
        self.upload_button.clicked.connect(self.upload_button_clicked)
        self.index_to_button[2] = self.upload_button

        self.cmd_button = QPushButton('CMD', self)
        font = self.cmd_button.font()
        font.setPointSize(60)
        self.cmd_button.setFont(font)
        self.cmd_button.setFixedSize(self.button_w, self.button_h)
        self.cmd_button.clicked.connect(self.cmd_button_clicked)
        self.index_to_button[3] = self.cmd_button

        self.buttons = [self.log_button, self.plot_button, self.upload_button, self.cmd_button]

        # Create a line edit for text input (hidden by default)
        self.line_edit = QLineEdit(self)
        self.line_edit.setPlaceholderText("Enter text here")
        self.line_edit.returnPressed.connect(self.save_text)
        self.line_edit.hide()

        self.buttons = [self.log_button, self.plot_button, self.upload_button, self.cmd_button]
        self.highlight_button()

        # Create a vertical layout for buttons
        button_layout = QVBoxLayout()
        button_layout.addWidget(self.log_button)
        button_layout.addWidget(self.plot_button)
        button_layout.addWidget(self.upload_button)
        button_layout.addWidget(self.cmd_button)
        button_layout.addStretch(1)

        # Create a vertical layout for the window
        layout = QVBoxLayout(self)
        layout.addWidget(self.status_label)
        layout.addLayout(button_layout)
        layout.addWidget(self.line_edit)

        # Connect to the logger
        self.logger = Log_Handler(self)
        self.portName = '/dev/tty.usbmodem3552356B32321'
        self.logger.openPort(self.portName)

        # Install event filter to handle key events
        self.installEventFilter(self)

    def eventFilter(self, obj, event):
        if event.type() == QEvent.KeyPress:
            key = event.key()
            if key == Qt.Key_Down:
                self.move_focus(1)
                return True
            elif key == Qt.Key_Up:
                self.move_focus(-1)
                return True
            elif key == Qt.Key_Right:
                self.trigger_button()
            elif key == Qt.Key_Return:
                if self.current_index == 3 and self.line_edit.hasFocus():
                    self.line_edit.clearFocus()
                else:
                    self.trigger_button()
                return True
        return super().eventFilter(obj, event)

    def open_second_window(self, text):
        self.second_window = CmdWindow()
        self.second_window.set_text(text)
        self.second_window.closing.connect(self.handle_second_window_close)
        self.second_window.show()

    def handle_second_window_close(self):
        self.activateWindow()

    def move_focus(self, direction):
        focus_widget = QApplication.focusWidget()
        self.current_index = (self.current_index + direction) % len(self.buttons)
        self.highlight_button()

    def checkStatus(self):
        for n, b in enumerate(self.buttons):
            if n == self.current_index:
                bg_color = self.highlight_color
                border_color = self.highlight_border
                if self.logger and not self.logger.port.isOpen():
                    print("not open")
                    bg_color = 'yellow'
            else:
                bg_color = self.normal_color
                border_color = self.normal_border

            if self.log_is_on and n == 0:
                bg_color = self.buttonColorGenerator(frequency=.4, amplitude=0.5, phase_shift=0, hue = 0.5) 

            b.setStyleSheet("background-color: {0};" "border :6px solid {1};".format(bg_color, border_color))

        if self.logger and not self.logger.port.isOpen():
            self.status_label.setText('Port open: error')


    def highlight_button(self):
        self.checkStatus()

    # colors to make get button throb
    def buttonColorGenerator(self, frequency, amplitude, phase_shift, hue):
        current_time = time.time()
        angle = (2 * math.pi * frequency * current_time) + phase_shift
        value = (math.sin(angle) + 1) / 2
        value *= amplitude
        saturation = 1.0
        lightness = (1 - value) - .2
        r, g, b = colorsys.hls_to_rgb(hue, lightness, saturation)
        html_color = "#{:02X}{:02X}{:02X}".format(int(r * 255), int(g * 255), int(b * 255))
        return html_color

    def trigger_button(self):
        focused_button = self.index_to_button[self.current_index]
        if focused_button == self.log_button:
            self.log_button_clicked()
        elif focused_button == self.plot_button:
            self.plot_button_clicked()
        elif focused_button == self.upload_button:
            self.upload_button_clicked()
        elif focused_button == self.cmd_button:
            self.cmd_button_clicked()

    def log_button_clicked(self):
        self.current_index = 0
        if self.log_is_on:
            self.logger.endDataLogging()
        else:
            self.logger.initDataLogging("log")

        self.log_is_on = not self.log_is_on
        self.highlight_button()
        self.status_label.setText("Logging initiated")

    def plot_button_clicked(self):
        self.current_index = 1
        self.highlight_button()

        if not self.logger.output_file or self.logger.output_file == '':
            self.status_label.setText("No log file to plot")
        else:
            self.status_label.setText("Showing plot")
            self.plot_file = self.logger.output_file
            self.matplotlib_window = PlotWindow(self)
            self.matplotlib_window.show()
            self.matplotlib_window.setFocus()
            self.matplotlib_window.closing.connect(self.handle_plot_window_close)

    def handle_plot_window_close(self):
        self.activateWindow()

    def upload_button_clicked(self):
        self.current_index = 2
        self.highlight_button()
        self.status_label.setText("Uploading initiated")

    def cmd_button_clicked(self):
        self.current_index = 3
        self.highlight_button()
        if self.line_edit.isHidden():
            self.line_edit.show()
            self.line_edit.setFocus()
            self.cmd_button.setText("type cmd text:")
        else:
            self.save_text()

    def save_text(self):
        text = self.line_edit.text()
        self.status_label.setText(f"entered: {text}")
        self.line_edit.clear()
        self.line_edit.hide()
        self.cmd_button.setText('CMD')
        self.logger.sendToPort(text)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = UI_Handler()
    window.show()
    sys.exit(app.exec_())
