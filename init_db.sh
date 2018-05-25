
#!/bin/bash

/etc/init.d/mysql start
sleep 1
mysql -e "CREATE USER 'radio'@'localhost' IDENTIFIED BY 'star11';"
mysql -e "CREATE DATABASE RADIO;"
mysql -e "GRANT SELECT,INSERT,UPDATE,DELETE,CREATE,DROP ON RADIO.* TO 'radio'@'localhost';"
mysql -e "GRANT FILE on *.* TO radio@localhost;"
mysql -e "CREATE TABLE RADIO.user_tbl (idx int(11) NOT NULL AUTO_INCREMENT, username text not null, password text not null, PRIMARY KEY (idx)) ENGINE=MyISAM DEFAULT CHARSET=utf8;"
mysql -e "CREATE TABLE RADIO.save_tbl (idx int(11) NOT NULL AUTO_INCREMENT, username text not null, data blob not null, PRIMARY KEY (idx)) ENGINE=MyISAM DEFAULT CHARSET=utf8;"

/etc/init.d/mysql stop
