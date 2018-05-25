#!/usr/bin/python
from common import *
from socket import socket, AF_INET, SOCK_STREAM
import bitarray
import numpy as np
import os
from tempfile import mkstemp
import pymysql
from struct import unpack


bp = 1 / float(1000000)
sp = bp * 2
duration = 100


def demodulate(d):
    o = []
    t = np.arange(sp/duration, sp + sp/duration, sp/duration, dtype=np.float)
    for i in range(0, len(d), duration):
        iF = np.multiply(np.cos(2*np.pi*t), d[i:i+duration])
        qF = np.multiply(np.sin(2*np.pi*t), d[i:i+duration])
        I = round(2 * np.trapz(iF)/sp)
        Q = round(2 * np.trapz(qF)/sp)
        o.extend([1 if I > 0 else 0, 1 if Q > 0 else 0])
    return bitarray.bitarray(o).tostring()


def handler_get():
    body = '<form method="post" action="">'
    body += '<input type="text" name="host" placeholder="IP Address"/>'
    body += '<input type="text" name="port" placeholder="Port"/>'
    body += '<input type="submit" value="Submit"/>'
    body += '</form>'
    main_tpl(body)


def handler_post():
    _POST = parse_str(POST)

    if 'host' not in _POST or 'port' not in _POST:
        redirect('/request.py#invalid+input')
        exit()

    host = str(_POST['host'])
    port = int(_POST['port'])

    username = SESS.get('username')
    if username == '':
        redirect('/request.py#invalid+session')
        exit()

    s = socket(AF_INET, SOCK_STREAM)
    s.connect((host, port))
    data = ''
    while True:
        temp = s.recv(4096)
        if temp == '': break
        data += temp
    s.close()

    if len(data) % 8 != 0:
        redirect('/request.py#invalid+data')
        exit()

    data = demodulate(np.array([unpack('<d',data[i:i+8])[0] for i in range(0,len(data),8)]))

    conn = pymysql.connect(host='localhost', user='radio', password='star11', db='RADIO',charset='utf8')
    try:
        with conn.cursor() as cursor:
            sql = "INSERT INTO save_tbl (username, data) VALUES (%s, %s);"
            cursor.execute(sql, (username, data))
        conn.commit()
    finally:
        conn.close()
    redirect('/request.py#success')


if __name__ == '__main__':
    if not SESS.get('logined'):
        redirect('index.py')
        exit()
    if METHOD == 'GET':
        handler_get()
    elif METHOD == 'POST':
        handler_post()
