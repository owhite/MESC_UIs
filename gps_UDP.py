#!/usr/bin/env python3

import socket
import pynmea2

class GNMEAReceiver:
    def __init__(self, ip_address, port):
        self.ip_address = ip_address
        self.port = port

    def start(self):
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
                s.bind((self.ip_address, self.port))
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

if __name__ == "__main__":
    ip_address = "192.16.54.210"
    ip_address = "" # actually just leave this empty
    port = 29999
    receiver = GNMEAReceiver(ip_address, port)
    receiver.start()
