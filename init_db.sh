#!/bin/bash

/usr/bin/mysqld_safe --skip-grant-tables&

sleep 3
mysql -uroot -e "UPDATE mysql.user SET plugin='mysql_native_password';"
sleep 1
mysql -uroot -e "UPDATE mysql.user SET password=PASSWORD('root') where User='root';"
sleep 1

python /init.py
killall mysqld_safe
