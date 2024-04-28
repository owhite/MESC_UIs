#!/usr/bin/env python3

#!/usr/bin/env python3

import paho.mqtt.client as mqtt

broker_address = "10.0.1.10"
broker_port = 1883

def on_connect1(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("#")

def on_message1(client, userdata, msg):
    print(f"Received message: {msg.payload.decode()}")

client = mqtt.Client()
client.on_connect = on_connect1
client.on_message = on_message1

client.connect(broker_address, broker_port, 60)
client.loop_forever()
