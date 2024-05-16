#!/usr/bin/env python3

import threading
import time

class ThreadOperation:
    def __init__(self):
        self._running = threading.Event()
        self._thread = None

    def _operation(self):
        while self._running.is_set():
            print("Working...")
            time.sleep(1)  # Simulate some work being done

    def start(self):
        if self._thread is None or not self._thread.is_alive():
            self._running.set()
            self._thread = threading.Thread(target=self._operation)
            self._thread.start()  # Start the thread in a non-blocking way
            print("Thread started")

    def stop(self):
        self._running.clear()
        if self._thread is not None:
            self._thread.join()  # Wait for the thread to finish
        print("Thread stopped")

    def threadIsRunning(self):
        return self._running.is_set() and self._thread is not None and self._thread.is_alive()

# Example usage
if __name__ == "__main__":
    ThreadInstance = ThreadOperation()
    
    ThreadInstance.start()
    print("1: Is thread running?", ThreadInstance.threadIsRunning())
    time.sleep(20)  # Let the thread run for 2 seconds
    ThreadInstance.stop()
    print("2: Is thread running?", ThreadInstance.threadIsRunning())
