#!/usr/bin/env python3

import socket
import pynmea2

ip_address = "192.16.54.210"
ip_address = ""
port = 29999

try:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.bind((ip_address, port))
        while True:
            data, addr = s.recvfrom(1024)
            if not data:
                continue  # Continue listening if no more data is received
            try:
                line = data.decode('utf-8')
                msg = pynmea2.parse(line)
                if hasattr(msg, 'latitude') and hasattr(msg, 'longitude'):
                    print(msg.latitude, msg.longitude)
            except pynmea2.ParseError as e:
                # Handle parsing errors here
                continue
except ConnectionRefusedError:
    print("Connection was refused.")
except TimeoutError:
    print("Connection timed out.")
except Exception as e:
    print("An error occurred:", e)
