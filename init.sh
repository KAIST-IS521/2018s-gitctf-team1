#!/bin/sh
/etc/init.d/mysql start
sleep 1
python /adduser.py
/hackttp/hackttp /var/www/cgi 4000 1>/dev/null 2>/dev/null
