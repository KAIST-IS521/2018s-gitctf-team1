#!/bin/sh
/etc/init.d/mysql start
/hackttp/hackttp /var/www/cgi 4000 1>/dev/null 2>/dev/null
