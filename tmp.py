#!/usr/bin/env python3
import sys
import logging
from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QVBoxLayout, QTextEdit, QTabWidget, QPushButton, QHBoxLayout

class SystemsWidget(QWidget, logging.Handler):
    def __init__(self, parent=None):
        super().__init__(parent)
        logging.Handler.__init__(self)

        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)
        self.text_edit.setLineWrapMode(QTextEdit.NoWrap)

        layout = QVBoxLayout()
        layout.addWidget(self.text_edit)

        self.setLayout(layout)
        self.current_buffer_size = 0
        self.max_buffer_size = 4000

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

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Main Window")

        # Set the width of the window
        self.resize(800, self.height())

        self.central_widget = QWidget()
        self.setCentralWidget(self.central_widget)

        self.tab_widget = QTabWidget()

        self.tab1 = QWidget()
        self.tab_widget.addTab(self.tab1, "Tab 1")

        self.systems_widget = SystemsWidget()

        self.button1 = QPushButton("Button 1")
        self.button2 = QPushButton("Button 2")

        layout = QHBoxLayout()
        layout.addWidget(self.button1)
        layout.addWidget(self.button2)

        self.tab_layout = QVBoxLayout()
        self.tab_layout.addLayout(layout)
        self.tab_layout.addWidget(self.systems_widget)

        self.tab1.setLayout(self.tab_layout)

        self.central_layout = QVBoxLayout()
        self.central_layout.addWidget(self.tab_widget)

        self.central_widget.setLayout(self.central_layout)

        # Creating a logger and adding the SystemsWidget handler
        self.logger = logging.getLogger('serial_service_log')
        self.logger.setLevel(logging.INFO)
        handler1 = self.systems_widget
        self.logger.addHandler(handler1)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
