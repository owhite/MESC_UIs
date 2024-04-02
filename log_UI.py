#!/usr/bin/env python3

import sys
import os
import time
import logging
import HostMessages
import time, math, colorsys

import GoogleHandler

import matplotlib
matplotlib.use('Qt5Agg')  # This chooses the appropriate backend for the RPI 

from PyQt5 import QtCore
from PyQt5.QtWidgets import QApplication, QWidget, QGridLayout, QPushButton, QHBoxLayout, QVBoxLayout
from PyQt5.QtWidgets import QMainWindow, QSizePolicy, QLabel, QLineEdit, QTextEdit, QTabWidget
from PyQt5.QtCore import Qt, QEvent, QTimer, pyqtSignal
from PyQt5.QtGui import QTextCursor, QPixmap, QFont

import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

# the goal of this code is to basically handle all the UI elements, while
#   cutting over the other serial stuff and logging stuff to other
#   python classes like LogHandler and GoogleHandler

class TopApplication(QMainWindow):
    def __init__(self):
        super().__init__()

        ##############################
        # system messages and serial messages
        self.msgs = HostMessages.LogHandler(self)

        # collect up some things
        if sys.platform.startswith('darwin'):
            self.msgs.logger.info("macOS detected")
            self.portName = '/dev/tty.usbmodem3552356B32321'
            account_file = "/Users/owhite/mesc-data-logging-ae144bcc6287.json"
        elif sys.platform.startswith('linux'):
            account_file = "/home/pi/mesc-data-logging-083b86e157cf.json"
            self.portName = '/dev/ttyACM0'
            self.msgs.logger.info("linux detected")
        else:
            self.msgs.logger.info("Unknown operating system")
            sys.exit()

        # open port, sometimes it's already streaming, shut it off
        self.msgs.openPort(self.portName)
        self.msgs.sendToPort('status stop') 

        # manage the uploader
        self.drive = GoogleHandler.handler(self.msgs.logger, account_file)
        self.msgs.logger.info("GoogleHandler initiated")

        self.output_data_file = 'MESC_logdata.txt'
        self.output_plot_file = 'MESC_plt.png'

        self.button_w = 600 - 20
        self.button_h = 80

        self.setGeometry(1, 1, self.button_w + 20, 350)
        self.setWindowTitle('MESC logger')

        self.tabs = QTabWidget()

        self.tab1 = firstTab(self)
        self.systemsTab = SystemsTab()
        self.serialOutTab = SerialOutTab(self)

        # now that tabs are made you can pass messages to them
        handler1 = self.systemsTab
        self.msgs.logger.addHandler(handler1)

        handler2 = self.serialOutTab
        self.msgs.serial_msgs.addHandler(handler2)

        # Add each tab to the QTabWidget
        self.tabs.addTab(self.tab1, "LOG")
        self.tabs.addTab(self.serialOutTab, "SER")
        self.tabs.addTab(self.systemsTab, "MSGS")

        self.setCentralWidget(self.tabs)

        # Install event filter to handle key events
        self.installEventFilter(self)


    def keyPressEvent(self, event):
        key = event.key()
        print(f"Key Pressed in Main Window: {key}")

# the HostMessaging class creates system logs and opens the serial to talk to controller
#  this tab takes output from the serial messages that are not used in logging
class SerialOutTab(QMainWindow, logging.Handler):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.parent = parent
        self.msgs = self.parent.msgs

        self.setWindowTitle("Log Viewer")

        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)
        self.text_edit.setLineWrapMode(QTextEdit.LineWrapMode.NoWrap)

        layout = QVBoxLayout()
        layout.addWidget(self.text_edit)

        # Input text that will be used as comments of log
        self.line_edit = QLineEdit(self)
        self.line_edit.setFont(QFont("Arial", 30))
        self.line_edit.setFixedHeight(40)
        self.line_edit.setPlaceholderText("Serial cmd")
        self.line_edit.returnPressed.connect(self.process_input_text)
        self.line_edit.mousePressEvent = self.on_line_edit_click

        layout.addWidget(self.line_edit)

        widget = QWidget()
        widget.setLayout(layout)
        self.current_buffer_size = 0
        self.max_buffer_size = 4000

        self.setCentralWidget(widget)

    def on_line_edit_click(self, event):
        print("Line Edit clicked!")

        super().mousePressEvent(event)

    def process_input_text(self):
        text = self.line_edit.text()
        self.line_edit.clear()
        self.msgs.sendToPort(text)

    def emit(self, record):
        msg = self.format(record)
        self.append_text(msg)

    def append_text(self, text):
        self.text_edit.append(text)
        self.current_buffer_size += len(text)
        if self.current_buffer_size > self.max_buffer_size:
            current_text = self.text_edit.toPlainText()
            excess_text = self.current_buffer_size - self.max_buffer_size
            trim_index = 0
            for i, c in enumerate(current_text):
                excess_text -= sys.getsizeof(c.encode())
                if excess_text <= 0:
                    trim_index = i
                    break
            trimmed_text = current_text[trim_index:]
            self.text_edit.setPlainText(trimmed_text)
            self.current_buffer_size = len(trimmed_text)
        
        # Move cursor to the end of the document
        cursor = self.text_edit.textCursor()
        cursor.movePosition(QTextCursor.End)
        self.text_edit.setTextCursor(cursor)

# the HostMessaging class creates system logs and opens the serial to talk to controller
#  this tab takes output from the SYSTEMS log
class SystemsTab(QMainWindow, logging.Handler):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Log Viewer")
        self.setGeometry(100, 100, 800, 600)

        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)
        self.text_edit.setLineWrapMode(QTextEdit.LineWrapMode.NoWrap)

        layout = QVBoxLayout()
        layout.addWidget(self.text_edit)

        widget = QWidget()
        widget.setLayout(layout)
        self.current_buffer_size = 0
        self.max_buffer_size = 4000

        self.setCentralWidget(widget)

    def emit(self, record):
        msg = self.format(record)
        self.append_text(msg)

    def append_text(self, text):
        self.text_edit.append(text)
        self.current_buffer_size += len(text)
        if self.current_buffer_size > self.max_buffer_size:
            current_text = self.text_edit.toPlainText()
            excess_text = self.current_buffer_size - self.max_buffer_size
            trim_index = 0
            for i, c in enumerate(current_text):
                excess_text -= sys.getsizeof(c.encode())
                if excess_text <= 0:
                    trim_index = i
                    break
            trimmed_text = current_text[trim_index:]
            self.text_edit.setPlainText(trimmed_text)
            self.current_buffer_size = len(trimmed_text)
        
        # Move cursor to the end of the document
        cursor = self.text_edit.textCursor()
        cursor.movePosition(QTextCursor.End)
        self.text_edit.setTextCursor(cursor)

class firstTab(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.parent = parent
        self.button_w = self.parent.button_w
        self.button_h = self.parent.button_h
        self.output_data_file = self.parent.output_data_file
        self.output_plot_file = self.parent.output_plot_file
        self.msgs = self.parent.msgs
        self.drive = self.parent.drive

        # Ensure this receives key events
        self.setFocusPolicy(Qt.StrongFocus)

        self.normal_color = "#FDFEFE"
        self.normal_border = "#BDC3C7"
        self.highlight_color = "#85C1E9" 
        self.highlight_border = "#229954"
        self.current_index = 0;
        self.index_to_widget = {}

        self.output_data_file = 'MESC_logdata.txt'
        self.output_plot_file = 'MESC_plt.png'

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.checkStatus)
        self.timer.start(50)
        self.log_is_on = False
        self.upload_thread = None

        self.init_ui()

    def init_ui(self):
        # Set window geometry

        # Central widget for main layout
        central_widget = QWidget(self)
        self.setCentralWidget(central_widget)

        # Label at top to display stuff
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
        self.log_button.setFocusPolicy(Qt.NoFocus)
        self.log_button.clicked.connect(self.log_button_clicked)
        self.index_to_widget[0] = self.log_button

        self.show_button = QPushButton('SHOW', self)
        font = self.show_button.font()
        font.setPointSize(60)
        self.show_button.setFont(font)
        self.show_button.setFocusPolicy(Qt.NoFocus)
        self.show_button.clicked.connect(self.show_button_clicked)
        self.index_to_widget[1] = self.show_button

        self.upload_button = QPushButton('UPLOAD', self)
        font = self.upload_button.font()
        font.setPointSize(60)
        self.upload_button.setFont(font)
        self.upload_button.setFocusPolicy(Qt.NoFocus)
        self.upload_button.clicked.connect(self.upload_button_clicked)
        self.index_to_widget[2] = self.upload_button

        # Input text that will be used as comments of log
        self.line_edit = QLineEdit(self)
        self.line_edit.setFont(QFont("Arial", 30))
        self.line_edit.setFixedHeight(40)
        self.line_edit.setPlaceholderText("Logging note")
        self.line_edit.returnPressed.connect(self.process_input_text)
        self.line_edit.mousePressEvent = self.on_line_edit_click

        layout = QGridLayout()

        layout.addWidget(self.status_label, 0, 0, 1, 3)
        layout.addWidget(self.log_button, 1, 0)
        layout.addWidget(self.show_button, 1, 1)
        layout.addWidget(self.upload_button, 1, 2)
        layout.addWidget(self.line_edit, 2, 0, 1, 3)

        # Set size policy for buttons
        self.log_button.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.show_button.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.upload_button.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)

        # Apply the main layout to the central widget
        central_widget.setLayout(layout)

        self.widgets = [self.log_button, self.show_button, self.upload_button]

    def on_line_edit_click(self, event):
        # Perform action when QLineEdit is clicked
        print("Line Edit clicked!")
        self.current_index = 3
        self.checkStatus()
        super().mousePressEvent(event)

    def keyPressEvent(self, event):
        key = event.key()
        print(f"Key Pressed in Log Tab: {key}")

        if key == Qt.Key_Down:
            if self.current_index == 0:
                self.current_index = 2
            elif self.current_index == 1:
                self.current_index = 3
            elif self.current_index == 2:
                self.current_index = 0
            elif self.current_index == 3:
                self.current_index = 1
            self.highlight_widget()
        elif key == Qt.Key_Up:
            if self.current_index == 0:
                self.current_index = 2
            elif self.current_index == 1:
                self.current_index = 3
            elif self.current_index == 2:
                self.current_index = 0
            elif self.current_index == 3:
                self.current_index = 1
            self.highlight_widget()
        elif key == Qt.Key_Right:
            if self.current_index == 0:
                self.current_index = 1
            elif self.current_index == 1:
                self.current_index = 2
            elif self.current_index == 2:
                self.current_index = 3
            elif self.current_index == 3:
                self.current_index = 0
            self.highlight_widget()
        elif key == Qt.Key_Left:
            if self.current_index == 0:
                self.current_index = 3
            elif self.current_index == 1:
                self.current_index = 0
            elif self.current_index == 2:
                self.current_index = 1
            elif self.current_index == 3:
                self.current_index = 2
            self.highlight_widget()
        elif key == Qt.Key_Return:
            if self.line_edit.hasFocus():
                # self.line_edit.clear()
                print(F"line edit: {self.line_edit.text()}")

            if self.current_index == 3 and self.line_edit.hasFocus():
                self.current_index = 0
                self.line_edit.clearFocus()
            else:
                self.trigger_button()

    def handle_second_window_close(self):
        self.activateWindow()

    def checkStatus(self):
        self.highlight_widget()
        if self.msgs and not self.msgs.port.isOpen():
            self.status_label.setText('Port open: error')

    def highlight_widget(self):
        for n, b in enumerate(self.widgets):
            if n == self.current_index:
                bg_color = self.highlight_color
                border_color = self.highlight_border
                if self.msgs and not self.msgs.port.isOpen():
                    bg_color = 'yellow'
            else:
                bg_color = self.normal_color
                border_color = self.normal_border

            if self.log_is_on and n == 0:
                bg_color = self.buttonColorGenerator(frequency=.4, amplitude=0.5, phase_shift=0, hue = 0.2) 

            if self.upload_thread is not None and self.upload_thread.is_alive() and n == 2:
                bg_color = self.buttonColorGenerator(frequency=.4, amplitude=0.5, phase_shift=0, hue = 0.35) 

            b.setStyleSheet("background-color: {0};" "border :6px solid {1};".format(bg_color, border_color))

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
        focused_button = self.index_to_widget[self.current_index]
        if focused_button == self.log_button:
            self.log_button_clicked()
        elif focused_button == self.show_button:
            self.show_button_clicked()
        elif focused_button == self.upload_button:
            self.upload_button_clicked()

    def log_button_clicked(self):
        self.current_index = 0
        if self.log_is_on:
            self.msgs.endDataLogging(self.output_plot_file)
        else:
            self.msgs.initDataLogging(self.output_data_file)

        self.log_is_on = not self.log_is_on
        self.highlight_widget()
        self.status_label.setText("Logging initiated")

    def handle_plot_window_close(self):
        self.activateWindow()

    def process_input_text(self):
        text = self.line_edit.text()
        self.status_label.setText(f"entered: {text}")

    def upload_button_clicked(self):
        self.current_index = 2
        self.highlight_widget()

        if not self.output_data_file or self.output_data_file == '':
            self.status_label.setText("No log file to upload")
        else:
            self.status_label.setText(F"Making plot")
            print(F"STARTING THREAD {self.line_edit.text()}")

            self.upload_thread = GoogleHandler.uploadThread(self, [self.output_data_file, self.output_plot_file], self.line_edit.text())
            self.upload_thread.start()

    def handle_plot_window_close(self):
        self.activateWindow()

    def show_button_clicked(self):
        self.current_index = 1
        self.highlight_widget()

        if not self.output_data_file or self.output_data_file == '':
            self.status_label.setText("No log file to plot")
        else:
            # xxx
            self.plot_window = PlotWindow(self.output_plot_file)
            self.plot_window.closing.connect(self.handle_plot_window_close)
            self.plot_window.show()

class PlotWindow(QWidget):
    closing = pyqtSignal()

    def __init__(self, input_file):
        super().__init__()

        print(self)
        self.setWindowTitle('New Window')
        self.setGeometry(1, 1, 500, 200)

        layout = QVBoxLayout()

        self.label = QLabel()
        self.label.setText('')
        layout.addWidget(self.label)

        if os.path.exists(input_file):
            pixmap = QPixmap(input_file)
            self.label.setPixmap(pixmap)
        else:
            self.label.setText("Image file not found!")

        self.setLayout(layout)

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Escape:
            self.close()
        if event.key() == Qt.Key_Return:
            self.close()

    def closeEvent(self, event):
        self.closing.emit()
        super().closeEvent(event)

class CmdWindow(QWidget):
    closing = pyqtSignal()

    def __init__(self):
        super().__init__()

        self.setWindowTitle('Second Window')
        self.setGeometry(1, 1, 800, 200)

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


if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = TopApplication()
    window.show()
    sys.exit(app.exec_())
