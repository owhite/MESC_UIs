#!/usr/bin/env python3

import paho.mqtt.client as mqtt
import random

# Define callback functions for MQTT events
def on_connect2(client, userdata, flags, reason_code, properties):
    if flags.session_present:
        pass
    if reason_code == 0:
        pass
    if reason_code > 0:
        pass

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT broker")
        # Subscribe to the topic of interest
        client.subscribe("ab")
    else:
        print("Failed to connect to MQTT broker with error code", rc)

def on_message(client, userdata, msg):
    print("Received message:", msg.payload.decode())

def on_disconnect(client, userdata, rc):
    if rc != 0:
        print("Unexpected disconnection from MQTT broker")

# Create an MQTT client instance
client_id = f'python-mqtt-{random.randint(0, 1000)}'
CLIENT_ID = f'python-mqtt-{random.randint(0, 1000)}'

client = mqtt.Client()

# Set callback functions for MQTT events
client.on_connect = on_connect
client.on_message = on_message
client.on_disconnect = on_disconnect

# Set username and password (if required by the MQTT broker)
username = "owhite"
password = "thing12"
client.username_pw_set(username, password)

# Connect to the MQTT broker
try:
    client.connect('localhost', 1883, 60)
    client.loop_forever()
except ConnectionRefusedError:
    print("Connection to MQTT broker refused")
except Exception as e:
    print("Error connecting to MQTT broker:", str(e))
