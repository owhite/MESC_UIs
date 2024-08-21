# Introduction

The purpose of this repo is to demonstrate connections between python programs with the [MESC](https://github.com/davidmolony/MESC_Firmware) motor controller. One of the programs can be run as a headless server on an raspberry PI, and facilitate interactions between the MESC controller and an android app. For the most part, you would probably want to use these to log data coming from the MESC, but there might be other applications for these programs as well. 

Note: MESC firmware was developed by David Molony, and the terminal and CAN bus communications of MESC was created by [Jens Kerrinnes](https://github.com/Netzpfuscher) Special thanks to both David and Jens. 

If you're a fan of the VESC motor controller project, it is conceivable that these could be adapted to work with that system as well but that is untested at this point. 

At present the functional programs in this repo are:
 * MESC Web Cal, the ESP32 Web Server 
 * app.py runs with 'python3 app.py' and makes a fairly functional web page
 * vert.py is PYQT5 app that can be run using VNC on your android
 * headless.py responds to the MQTT android app ([MQTT APP](https://github.com/owhite/MqttApp))
  
headless.py is probably broken at this point

# MESC Web Cal, the ESP32 Web Server 

Running a webserver on the ESP32. Results are preliminary, have a look at short video:

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/E1SCVV9bBj0/0.jpg)](https://www.youtube.com/watch?v=E1SCVV9bBj0)

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
My MESC code is set up to connect to UART3. 

Install the ESP32/webserver code using platformio:

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

## ESP32 Serial interactions
The ESP32 chip is presumably connected to a USB, and the webserver will stream information to the serial. Connect it up, and platformio now has a great serial monitor:

```
$ pio device monitor
Connecting to wifi....
WiFi connected
IP address: 10.0.1.59
HTTP server started
```

Open your browswer, go to the IP address: 10.0.1.59, a page will load showing some mesc variables.

Getting output from the serial on connection does not always work. Sometimes the webserver continues running after reboot, so it does not always report it's IP address. The default is the ESP32 USB connection will report debugging information, but if you type the '>' character you can launch a help session. The ESP32-terminal is a WIP, but if you type:
```
>
Switching to ESP32 Configuration
IP
IP address: 10.0.1.62
```

If you type 'IP' the web server reports it's address. 

## ESP32 wifi connection
config.txt contains details for the wifi:

```
# WiFi settings
ssid=YOUR_SSID_HERE
password=YOUR_PASSWORD_HERE

```

It is also possible to set the network and password using the command/serial connection, but that workflow is likely to change. 

## ESP32 compilation - a note about the files used by pio

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

## ESP32 Javascript Graphing
One enjoyable thing about webservers is the client can do a lot of the work for you -- such as graphing.

There are two variants of the graphing methods that have been tested so far. 
* [webserver](ESP32/webserver/) is based on [https://github.com/SK-SpeedBit/js_chart](https://github.com/SK-SpeedBit/js_chart)
* [chart_js](ESP32/chart_js/) is based on [https://www.chartjs.org](https://www.chartjs.org)

The code from these sites help with charting the results of 'log -fl' from [Jens term](https://github.com/Netzpfuscher). This code seems to work with Chrome on my mac as well as android. 

My favorite graphing library is plot.ly, and [this](https://owhite.github.io/) is an example of a toy-rendering of a MESC output using that library. The example shown there is static data embedded directly in the page. Plotly does a nice job of handling zooming and mouseovers. 

## Code issues

Items for improvement:
 * Very little debugging information
 ** Serial command line codes are very limited
 ** Bluetooth communication would be helpful
 ** [ESP32/i2c_testing](ESP32/i2c_testing/) is a boring oled display
 * CAN bus support would be cool

# Raspberry PI: MESC UIs

The other programs in this repo are just toys and unlikely to work straight out of the box. You will need working knowledge of:
 * python and installing python modules
 * Message Queuing Telemetry Transport (MQTT) and mosquitto
 * ...and probably android app installation

These programs:
 * send commands over the USB-serial to the MESC controller
 * send commands or grab user parameters embedded in the controller
 * initiate streams of json data containing real time controller information
 * split out streams of data coming from the controller for logging
 * combine GPS data into the logging stream
 * create matplotlib plots of the data streams
 * handle uploads of both the data and plots to google drive
 
In general implementation of the code would occur on a raspberry PI (RPI) that is connected to the MESC controller over USB serial and physically traveling with the controller during logging. Instead of using the android as an interface it's conceivable to put a touch screen on the RPI -- I tried using an inexpensive touchscreen on the RPI and hated it. Also, in comparison to the ESP32, RPI work is not very interesting. These programs do work nicely on a computer command line for a MESC-configuration system, but so does Jens' term. 


