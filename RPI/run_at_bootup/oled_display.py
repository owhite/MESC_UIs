import time
import board
import digitalio
from datetime import datetime
from PIL import Image, ImageDraw, ImageFont
import adafruit_ssd1306
import subprocess

oled_reset = digitalio.DigitalInOut(board.D4)

WIDTH = 128
HEIGHT = 64
BORDER = 5

i2c = board.I2C()
oled = adafruit_ssd1306.SSD1306_I2C(WIDTH, HEIGHT, i2c, addr=0x3C, reset=oled_reset)

# Clear display.
oled.fill(0)
oled.show()

image = Image.new("1", (oled.width, oled.height))
draw = ImageDraw.Draw(image)
draw.rectangle((0, 0, oled.width, oled.height), outline=255, fill=255)
font = ImageFont.truetype('/home/pi/RPI_logger/run_at_bootup/PixelOperator.ttf', 32)

loop_duration = 1  # in seconds

def main():

    now = datetime.now()
    formatted_date = now.strftime("%Y-%m-%d %H:%M:%S")
    toggle = True
    loop_time = time.time() + loop_duration
    while True:
        if time.time() >= loop_time:
            loop_time += loop_duration
            draw.rectangle((0, 0, oled.width, oled.height), outline=0, fill=0)

            cmd = "hostname -I | cut -d\' \' -f1"
            IP = subprocess.check_output(cmd, shell = True )
            IP = str(IP,'utf-8')

            if len(IP) > 5: 
                l = IP.split('.')
                draw.text((0, 0),   l[0], font=font, fill=255)
                draw.text((60, 0),  l[1], font=font, fill=255)
                draw.text((0, 30),  l[2], font=font, fill=255)
                draw.text((60, 30), l[3], font=font, fill=255)
                if toggle:
                    draw.text((110, 30), "*", font=font, fill=255)
                else:
                    draw.text((110, 30), " ", font=font, fill=255)
            else:
                draw.text((0, 0), "no IP", font=font, fill=255)
                draw.text((0, 30), "found", font=font, fill=255)
    
            toggle = not toggle
            oled.image(image)
            oled.show()
    
    # clear of while loop
    print("Stats program finished executing.")

if __name__ == "__main__":
    main()

