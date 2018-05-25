#!/usr/bin/python
from common import render_base, parse_str, redirect, get_template
from common import SESS, POST, METHOD
from socket import socket, AF_INET, SOCK_STREAM
import bitarray
import numpy as np
import pymysql
from struct import unpack


def demodulate(d):
    bp = 1 / float(1000000)
    sp = bp * 2
    duration = 100
    out = []
    t = np.arange(sp/duration, sp + sp/duration, sp/duration, dtype=np.float)
    for i in range(0, len(d), duration):
        iF = np.multiply(np.cos(2*np.pi*t), d[i:i+duration])
        qF = np.multiply(np.sin(2*np.pi*t), d[i:i+duration])
        I = round(2 * np.trapz(iF)/sp)
        Q = round(2 * np.trapz(qF)/sp)
        out.extend([1 if I > 0 else 0, 1 if Q > 0 else 0])
    return bitarray.bitarray(out).tostring()


def handler_get():
    render_base(get_template('request.tpl'))


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
        if temp == '':
            break
        data += temp
    s.close()

    if len(data) % 8 != 0:
        redirect('/request.py#invalid+data')
        exit()

    data = demodulate(np.array([unpack('<d', data[i:i+8])[0]
                                for i in range(0, len(data), 8)]))

    conn = pymysql.connect(host='localhost', user='radio', password='star11',
                           db='RADIO', charset='utf8')
    try:
        sql = "INSERT INTO save_tbl (username, data) VALUES ('{}', '{}');".\
                format(username, data)
        with conn.cursor() as cursor:
            cursor.execute(sql)
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
