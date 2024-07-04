import time
from datetime import datetime
import subprocess
import threading

class OledDisplay:
    def __init__(self):
        self.loop_duration = 1  # in seconds
        self.running = False

    def display_ip(self):
        self.running = True
        now = datetime.now()
        formatted_date = now.strftime("%Y-%m-%d %H:%M:%S")
        toggle = True
        loop_time = time.time() + self.loop_duration
        print("starting to run", formatted_date)

        while self.running:
            if time.time() >= loop_time:
                loop_time += self.loop_duration

                toggle = not toggle

                print("TOGGLE", toggle)

        print("Stats program finished executing.")

    def stop_display(self):
        self.running = False
