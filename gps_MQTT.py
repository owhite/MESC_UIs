#!/usr/bin/env python3

import paho.mqtt.client as mqtt

broker_address = "192.16.54.71"
broker_port = 1883

def on_connect_wrapper(client, userdata, flags, rc, additional_param):
    on_connect(client, userdata, flags, rc, additional_param)

def on_connect(client, userdata, flags, rc, additional_param):
    print("Connected with result code "+str(rc))
    print("Additional parameter:", additional_param)
    client.subscribe("#") 

def on_message(client, userdata, msg):
    print(f"Received message: {msg.payload.decode()}")

client = mqtt.Client()
additional_param = "Hello from additional parameter!"
client.on_connect = lambda client, userdata, flags, rc: on_connect_wrapper(client, userdata, flags, rc, additional_param)
client.on_message = on_message

client.connect(broker_address, broker_port, 60)

client.loop_forever()
