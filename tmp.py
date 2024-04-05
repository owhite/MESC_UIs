#!/usr/bin/env python3

import subprocess

process = subprocess.Popen(['/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport','-I'], stdout=subprocess.PIPE)
out, err = process.communicate()
process.wait()

network_info_str = out.decode('utf-8')

# Split the string into lines
lines = network_info_str.split('\n')

# Iterate through each line
d = {}
for line in lines:
    line = line.lstrip()
    if ":" in line:
        l = line.split(':')
        d[l[0].lstrip()] = l[1].lstrip()

if d.get('SSID'):
    print (d['SSID'])
