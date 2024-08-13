# Introduction

The purpose of this repo is to demonstrate connections between python programs with the [MESC](https://github.com/davidmolony/MESC_Firmware) motor controller. One of the programs can be run as a headless server on an raspberry PI, and facilitate interactions between the MESC controller and an android app. For the most part, you would probably want to use these to log data coming from the MESC, but there might be other applications for these programs as well. 

Note: MESC firmware was developed by David Molony, and the terminal and CAN bus communications of MESC was created by [Jens Kerrinnes](https://github.com/Netzpfuscher) Special thanks to both David and Jens. 

The programs in this repo are just toys and unlikely to work straight out of the box. You will need working knowledge of:
 * python and installing python modules
 * [MESC](https://github.com/davidmolony/MESC_Firmware)
 * Message Queuing Telemetry Transport (MQTT) and mosquitto
 * ...and probably android app installation

If you're a fan of the VESC motor controller project, it is conceivable that these could be adapted to work with that system as well but that is untested at this point. 

At present the functional programs in this repo are:
 * MESC Web Cal, the ESP32 Web Server 
 * app.py runs with 'python3 app.py' and makes a fairly functional web page
 * vert.py is PYQT5 app that can be run using VNC on your android
 * headless.py responds to the MQTT android app ([MQTT APP](https://github.com/owhite/MqttApp))
  
headless.py is probably broken at this point

# MESC Web Cal, the ESP32 Web Server 

Running a webserver on the ESP32. Results are preliminary. Install the ESP32/webserver code using platformio:

The ESP32 pins that connect to the serial are defined into main.cpp:
```
#define TX 0
#define RX 1

HardwareSerial mescSerial(1);
.
.
.
mescSerial.begin(115200, SERIAL_8N1, RX, TX); 
```

```
$ cd ESP32/webserver/
$ pio run 
```

Well, that would be great if it works. You will  need to adjust the platformio.ini file:
```
[env:esp32-c3-devkitm-1]
platform = espressif32
board = esp32-c3-devkitm-1
framework = arduino
monitor_speed = 115200
board_build.filesystem = littlefs
targets = upload

lib_deps =
    ArduinoJson@6.18.0 ;
    me-no-dev/AsyncTCP@^1.1.1 ; 
    me-no-dev/ESP Async WebServer@^1.2.4 ; 

extra_scripts = pre:extra_script.py

```
Notice the board configuration is set to work with an ESP32-C3. Other configs may be reqiured. 

## Serial interactions
The ESP32 chip is presumably connected to a USB, and the webserver will stream information to the serial. Connect it up, and platformio now has a great serial monitor:

```
$ pio device monitor
Connecting to wifi....
WiFi connected
IP address: 10.0.1.59
HTTP server started
```

Open your browswer, go to the IP address: 10.0.1.59, a page will load showing some mesc variables.

THIS WILL NOT ALWAYS WORK. Sometimes the webserver continues running after reboot, so it does not always report it's IP address. The default is the ESP32 USB connection will report debugging information, but if you type:
```
>
```
you can launch a help session. The ESP32-terminal is a WIP, but if you type:
```
>
Switching to ESP32 Configuration
IP
IP address: 10.0.1.62
```

the web server reports on it's IP

## A note about the files used by pio

```
webserver/
├── .pio/                  # PlatformIO build and project files
│   ├── .pioenvs/          # Environment-specific build folders
│   └── libdeps/           # Library dependencies for the project
├── src/                   # Source code files
│   ├── main.cpp           # Main application file
│   ├── other_files.cpp    # Other source files
│   └── index_html.h       # Discussed below
├── data/                  # Supports LittleFS
│   ├── config.txt         # Contains password / wifi information
│   └── index.html         # Converted to index_html.h
├── include/               # Never used 
├── lib/                   # Never used
├── platformio.ini         # PlatformIO configuration file
├── extra_script.py        # Helps with pre-preprocessing files
├── generate_header.py     # Helps with pre-preprocessing files
├── README.md              # Project documentation
└── .gitignore             # Git ignore file
```

when you perform 'pio run' it handles some cool things:
* it makes sure you have libraries needed, like ArduinoJson, AsyncTCP
* extra_script.py uploads config.txt to the ESP32
* generate_header.py converts index.html into a header for the web client. 

## Special shout out to the javascript graph code
https://github.com/SK-SpeedBit/js_chart/tree/master

# Raspberry PI support code

These python programs 
 * send commands over the USB-serial to the MESC controller
 * send commands or grab user parameters embedded in the controller
 * initiate streams of json data containing real time controller information
 * split out streams of data coming from the controller for logging
 * combine GPS data into the logging stream
 * create matplotlib plots of the data streams
 * handle uploads of both the data and plots to google drive
 
In general implementation of the code would occur on a raspberry PI (RPI) that is connected to the MESC controller over USB serial and physically traveling with the controller during logging. Instead of using the android as an interface it's conceivable to put a touch screen on the RPI -- I tried using an inexpensive touchscreen on the RPI and hated it. 

So if you're running a PI and want to connect to it using your smartphone you'll have to get it connect as a hotspot, and if your phone is like mine it wont even tell you the IP address of the PI. That's where this little oled display program comes in. Connect it to the I2C pins of the PI then...

## test the i2c connection:
$ i2cdetect -y 1

## Launch the progam using supervisor:
$ cat > /etc/supervisor/conf.d/IP_stats.conf
```[program:IP_stats]
command=/usr/bin/python3 /home/pi/RPI_logger/run_at_bootup/oled_stats.py
autostart=true  
autorestart=true
startsecs=5     
stderr_logfile=/var/log/IP_stats.err.log
stdout_logfile=/var/log/IP_stats.out.log
```
Make this launch at boot:

$ sudo systemctl enable supervisor

If you need to restart:

$ sudo supervisorctl restart IP_stats

## Code cleanup:

Run this to remove unneeded python imports:

$ python3 /Library/Frameworks/Python.framework/Versions/3.12/lib/python3.12/site-packages/autoflake.py  --remove-all-unused-imports --in-place --recursive

