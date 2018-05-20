#/bin/sh
/etc/init.d/mysql start
python init.py
git clone https://github.com/KAIST-IS521/2018s-gitctf-team1
cd 2018s-gitctf-team1
cd hackttp
make
./hackttp ../Service
