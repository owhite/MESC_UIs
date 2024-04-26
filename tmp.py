#!/usr/bin/env python3

# self.loadSVG("FP_Satellite_icon.svg")

import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QLabel
from PyQt5.QtGui import QPixmap, QImage, QColor
from PyQt5.QtCore import Qt

class PNGWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.initUI()

    def initUI(self):
        self.setGeometry(100, 100, 400, 400)
        self.setWindowTitle('PNG Display')

        filepath = "wifi.png"
        self.wifiLabel = self.makePNGLabel(filepath)

        filepath = "satellite.png"
        self.satelliteLabel = self.makePNGLabel(filepath)

        # Change the color
        self.changePNGLabelColor(self.wifiLabel, Qt.red)

        self.show()

    def makePNGLabel(self, filepath):
        pngLabel = QLabel(self)
        pngLabel.setGeometry(0, 0, 64, 64)
        pixmap = QPixmap(filepath)
        pngLabel.setPixmap(pixmap)
        return pngLabel


    def changePNGLabelColor(self, label, color):
        pixmap = label.pixmap()
        
        image = pixmap.toImage()
        for y in range(image.height()):
            for x in range(image.width()):
                pixel_color = QColor(image.pixelColor(x, y))
                if pixel_color != Qt.white and pixel_color != Qt.transparent:
                    image.setPixelColor(x, y, color)

        new_pixmap = QPixmap.fromImage(image)
        label.setPixmap(new_pixmap)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = PNGWindow()
    sys.exit(app.exec_())
