#!/usr/bin/env python3

import sys
from PyQt5.QtWidgets import QApplication, QWidget, QLabel
from PyQt5.QtCore import Qt

class KeyPressEventWidget(QWidget):
    def __init__(self):
        super().__init__()

        self.setWindowTitle('Key Press Event')
        self.setGeometry(100, 100, 300, 200)

        self.label = QLabel('Press a key', self)
        self.label.setGeometry(50, 50, 200, 100)
        self.label.setAlignment(Qt.AlignCenter)

    def keyPressEvent(self, event):
        key = event.key()
        modifiers = event.modifiers()

        print(key)
        if modifiers & Qt.ControlModifier and key == Qt.Key_Q:
            QApplication.quit()
        else:
            self.label.setText(f'Key Pressed: {event.text()}')

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = KeyPressEventWidget()
    window.show()
    sys.exit(app.exec_())
