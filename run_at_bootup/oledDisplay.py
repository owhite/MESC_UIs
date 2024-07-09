import time
import board
import digitalio
from datetime import datetime
from PIL import Image, ImageDraw, ImageFont
import adafruit_ssd1306
import subprocess
import threading

class OledDisplay:
    def __init__(self):
        self.oled_reset = digitalio.DigitalInOut(board.D4)
        self.WIDTH = 128
        self.HEIGHT = 64
        self.BORDER = 5
        self.i2c = board.I2C()
        self.oled = adafruit_ssd1306.SSD1306_I2C(self.WIDTH, self.HEIGHT, self.i2c, addr=0x3C, reset=self.oled_reset)
        self.oled.fill(0)
        self.oled.show()
        self.image = Image.new("1", (self.oled.width, self.oled.height))
        self.draw = ImageDraw.Draw(self.image)
        self.draw.rectangle((0, 0, self.oled.width, self.oled.height), outline=255, fill=255)
        self.font = ImageFont.truetype('/home/pi/RPI_logger/run_at_bootup/PixelOperator.ttf', 32)
        self.loop_duration = 1  # in seconds
        self.running = False

    def display_ip(self):
        self.running = True
        toggle = True
        loop_time = time.time() + self.loop_duration
        while self.running:
            if time.time() >= loop_time:
                loop_time += self.loop_duration
                self.draw.rectangle((0, 0, self.oled.width, self.oled.height), outline=0, fill=0)

                cmd = "hostname -I | cut -d\' \' -f1"
                IP = subprocess.check_output(cmd, shell=True)
                IP = str(IP, 'utf-8')

                if len(IP) > 5:
                    l = IP.split('.')
                    self.draw.text((0, 0), l[0], font=self.font, fill=255)
                    self.draw.text((60, 0), l[1], font=self.font, fill=255)
                    self.draw.text((0, 30), l[2], font=self.font, fill=255)
                    self.draw.text((60, 30), l[3], font=self.font, fill=255)
                    if toggle:
                        self.draw.text((110, 30), "*", font=self.font, fill=255)
                    else:
                        self.draw.text((110, 30), " ", font=self.font, fill=255)
                else:
                    self.draw.text((0, 0), "no IP", font=self.font, fill=255)
                    self.draw.text((0, 30), "found", font=self.font, fill=255)

                toggle = not toggle
                self.oled.image(self.image)
                self.oled.show()

    def stop_display(self):
        self.running = False
        
