#!/usr/bin/env python3
import sys
import logging
from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QVBoxLayout, QTextEdit, QTabWidget, QPushButton, QHBoxLayout

class MyWidget(QWidget):
    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Enter:
            print("Enter key (keypad) pressed")

if __name__ == '__main__':
    app = QApplication([])
    widget = MyWidget()
    widget.show()
    app.exec_()
