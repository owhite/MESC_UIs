#!/usr/bin/env python3

import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QLabel
import paho.mqtt.client as mqtt
import time
import json

class mqttHandler:
    def __init__(self, broker_address, broker_port, topic, callback=None):
        self.broker_address = broker_address
        self.broker_port = broker_port
        self.topic = topic
        self.callback = callback  # Store the callback function

        self.client = mqtt.Client()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        self.message = ''
        self.msg_dict = None
        self._start_time = None

    def on_connect(self, client, userdata, flags, rc):
        client.subscribe(self.topic)

    def on_message(self, client, userdata, msg):
        # print(f"Received message: {msg.payload.decode()}")
        self.msg_dict = None
        try:
            self.msg_dict = json.loads(msg.payload.decode())
        except json.JSONDecodeError:
            pass

        if self.callback:
            self.callback(msg.payload.decode())

    def startTimer(self):
        """Start a new timer"""
        if self._start_time is not None:
            raise gps_MQTTError(f"timer is running. Use .stop() to stop it")

        self._start_time = time.perf_counter()

    def stopTimer(self):
        """Stop the timer, and report the elapsed time"""
        if self._start_time is None:
            raise gps_MQTTError(f"timer is not running. Use .start() to start it")

        elapsed_time = time.perf_counter() - self._start_time
        self._start_time = None

    def resetTimer(self):
        """Reset a new timer"""
        if self._start_time is None:
            raise gps_MQTTError(f"timer is not running. Use .start() to start it")

        self._start_time = time.perf_counter()


    def reportTimer(self):
        """return timer value"""
        if self._start_time is None:
            raise gps_MQTTError(f"timer is not running. Use .start() to start it")

        return(time.perf_counter() - self._start_time)

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.initUI()

    def initUI(self):
        self.setWindowTitle("MQTT Subscriber")
        self.setGeometry(100, 100, 400, 300)

        self.label = QLabel(self)
        self.label.setGeometry(50, 50, 300, 200)
        self.label.setText("Waiting for message...")
        self.count = 1

        broker_address = "10.0.1.10"
        broker_port = 1883
        topic = "#"

        self.mqttMsg = mqttHandler(broker_address, broker_port, topic, self.update_label)
        try:
            self.mqttMsg.client.connect(broker_address, broker_port, 60)
            self.mqttMsg.client.loop_start()
        except ConnectionRefusedError:
            print("bombed out")


    def update_label(self, message):
        self.count += 1
        try:
            json_dict = json.loads(message)
            self.label.setText(str(self.count) + " " + str(json_dict['lon']))
        except json.JSONDecodeError as e:
            print("Error loading JSON:", e)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    mainWindow = MainWindow()
    mainWindow.show()
    sys.exit(app.exec_())
