#!/usr/bin/env python3

import HostMessages
import sys
import logging
from PyQt5.QtWidgets import QApplication, QMainWindow, QTextEdit, QVBoxLayout, QWidget, QTabWidget
from PyQt5.QtCore import QTimer

class TopApplication(QMainWindow):
    def __init__(self):
        super().__init__()

        self.output_data_file = 'MESC_logdata.txt'
        self.output_plot_file = 'MESC_plt.png'

        self.button_w = 600 - 20
        self.button_h = 80

        self.setGeometry(1, 1, self.button_w + 20, 350)
        self.setWindowTitle('MESC logger')

        self.tabs = QTabWidget()

        self.systemsTab = SystemsTab()

        self.tabs.addTab(self.systemsTab, "MSGS")

        self.setCentralWidget(self.tabs)

        self.msgs = HostMessages.Log_Handler(self)
        new_handler = self.systemsTab
        self.msgs.logger.addHandler(new_handler)

        self.msgs.logger.info("INITIALIZING LOGGER")

# the HostMessaging class creates system logs and opens the serial to talk to controller
#  this tab output from the systems log
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
        self.setCentralWidget(widget)

    def emit(self, record):
        msg = self.format(record)
        self.text_edit.append("THING" + msg)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = TopApplication()
    window.show()
    sys.exit(app.exec_())
