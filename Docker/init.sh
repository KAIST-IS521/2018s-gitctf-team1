#!/bin/sh
/etc/init.d/mysql start
python /init.py
cd /hackttp && make
/hackttp/hackttp /var/www/cgi
