import time
from datetime import datetime
import threading

class OledDisplay:
    def __init__(self):
        self.loop_duration = 1  # in seconds
        self.running = False
        self.display_thread = None

    def display_ip(self):
        self.running = True
        self.display_thread = threading.Thread(target=self._display_ip_thread)
        self.display_thread.start()

    def _display_ip_thread(self):
        while self.running:
            now = datetime.now()
            formatted_date = now.strftime("%Y-%m-%d %H:%M:%S")
            toggle = True
            loop_time = time.time() + self.loop_duration
            print("Starting to run", formatted_date)

            while self.running:
                if time.time() >= loop_time:
                    loop_time += self.loop_duration
                    toggle = not toggle
                    # Example logic, replace with your actual display logic
                    print(f"Display toggle: {toggle}")

            print("Stats program finished executing.")

    def stop_display(self):
        self.running = False
        if self.display_thread:
            self.display_thread.join()  # Wait for thread to finish

if __name__ == "__main__":
    # Example usage:
    oled = OledDisplay()
    oled.display_ip()
    time.sleep(10)  # Let it run for 10 seconds
    oled.stop_display()
