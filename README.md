## Introduction

The purpose of this repo is to demonstrate connections between python programs with the [MESC](https://github.com/davidmolony/MESC_Firmware) motor controller. One of the programs can be run as a headless server on an raspberry PI, and facilitate interactions between the MESC controller and an android app. For the most part, you would probably want to use these to log data coming from the MESC, but there might be other applications for these programs as well. 

Note: MESC firmware was developed by David Molony, and the terminal and CAN bus communications of MESC was created by [Jens Kerrinnes](https://github.com/Netzpfuscher) Special thanks to both David and Jens. 

The programs in this repo are just toys and unlikely to work straight out of the box. You will need working knowledge of:
 * python and installing python modules
 * [MESC](https://github.com/davidmolony/MESC_Firmware)
 * Message Queuing Telemetry Transport (MQTT) and mosquitto
 * ...and probably android app installation

If you're a fan of the VESC motor controller project, it is conceivable that these could be adapted to work with that system as well but that is untested at this point. 

At present the three functional programs in this repo are app.py, vert.py and headless.py
  * app.py runs with 'python3 app.py' and makes a fairly functional web page
  * vert.py is PYQT5 app that can be run using VNC on your android
  * headless.py responds to the MQTT android app ([MQTT APP](https://github.com/owhite/MqttApp))
  
headless.py is probably broken at this point

In any case, these python programs 
 * send commands over the USB-serial to the MESC controller
 * send commands or grab user parameters embedded in the controller
 * initiate streams of json data containing real time controller information
 * split out streams of data coming from the controller for logging
 * combine GPS data into the logging stream
 * create matplotlib plots of the data streams
 * handle uploads of both the data and plots to google drive
 
In general implementation of the code would occur on a raspberry PI (RPI) that is connected to the MESC controller over USB serial and physically traveling with the controller during logging. Instead of using the android as an interface it's conceivable to put a touch screen on the RPI -- I tried using an inexpensive touchscreen on the RPI and hated it. 

# Raspberry PI support code

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

## ESP32

At present I got a webserver running on an ESP32. The results are quite preliminary. Install the ESP32/webserver code using platformio:

$ pio run 

## Using platformio
Platformio now has a great serial monitor:

```
$ pio device monitor
Connecting to wifi....
WiFi connected
IP address: 10.0.1.59
HTTP server started
```

Open your browswer, go to the IP address: 10.0.1.59, a page will load showing some mesc variables.

Right now the only thing that happens is it parses data that sort of looks like lines from the jen's terminal, for example:

```
adc1	0	0	4096	Raw ADC throttle
adc1_max	2700	0	4096	ADC1 max val
adc1_min	600	0	4096	ADC1 min val
adc1_pol	1.000000	-1.00	1.00	ADC1 polarity
adc2_max	4095	0	4096	ADC2 max val
adc2_min	1200	0	4096	ADC2 min val
adc2_pol	1.000000	-1.00	1.00	ADC2 polarity
can_adc	11	0	254	CAN ADC ID  0=disabled
curr_max	40.000000	0.00	300.00	Max motor current
curr_min	-10.000000	-300.00	0.00	Min motor current
direction	0	0	1	Motor direction
ehz	0.172152	-inf	inf	Motor electrical hz
enc_angle	0	0	65535	Encoder angle now
```

if you cut and paste this into the terminal, it gets parsed and loaded into the web page.

Definitely needs work. 

## Attribution to this charting code:
https://github.com/SK-SpeedBit/js_chart/tree/master