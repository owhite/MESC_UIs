# test the i2c connection:
$ i2cdetect -y 1

# OLED Stats
$ cat  /etc/supervisor/conf.d/IP_stats.conf
[program:IP_stats]
command=/usr/bin/python3 /home/pi/run_at_boot_up/stats.py
; autostart=true  
autorestart=true
startsecs=5     
stderr_logfile=/var/log/IP_stats.err.log
stdout_logfile=/var/log/IP_stats.out.log

Make this launches at boot:
$ sudo systemctl enable supervisor

If you need to restart:
$ sudo supervisorctl restart IP_stats