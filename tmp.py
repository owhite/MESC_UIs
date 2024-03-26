#!/usr/bin/env python3

import sys
import subprocess
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QPushButton, QTextEdit
from PyQt5.QtGui import QTextCursor
from PyQt5.QtCore import Qt, pyqtSignal

class SecondWindow(QWidget):
    closing = pyqtSignal()

    def __init__(self):
        super().__init__()

        self.setWindowTitle('Second Window')
        self.setGeometry(100, 100, 400, 300)

        layout = QVBoxLayout()

        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)
        layout.addWidget(self.text_edit)

        self.setLayout(layout)

        self.populate_text()

    def populate_text(self):
        result = subprocess.run(['ls', '-l'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if result.returncode == 0:
            self.text_edit.setPlainText(result.stdout)
        else:
            self.text_edit.setPlainText("Error running 'ls' command.")

        # Scroll to the top of the text
        cursor = self.text_edit.textCursor()
        cursor.movePosition(QTextCursor.Start)
        self.text_edit.setTextCursor(cursor)

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Escape:
            self.close()

    def closeEvent(self, event):
        self.closing.emit()
        super().closeEvent(event)


class MainWindow(QWidget):
    def __init__(self):
        super().__init__()

        self.setWindowTitle('Main Window')
        self.setGeometry(100, 100, 400, 200)

        layout = QVBoxLayout()

        self.open_button = QPushButton('Open Second Window')
        self.open_button.clicked.connect(self.open_second_window)
        layout.addWidget(self.open_button)

        self.setLayout(layout)

    def open_second_window(self, text):
        self.second_window = SecondWindow()
        self.second_window.set_text(text)
        self.second_window.closing.connect(self.handle_second_window_close)
        self.second_window.show()

    def handle_second_window_close(self):
        self.activateWindow()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
