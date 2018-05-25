#!/usr/bin/python
from common import SESS, POST, METHOD
from common import parse_str, redirect, render_form
from RSA import enc
import pymysql
import sys

def handler_post():
    _POST = parse_str(POST)
    if 'username' not in _POST or 'password' not in _POST:
        redirect('/join.py')
        exit()

    username = str(_POST['username'])
    password = enc(str(_POST['password']))

    conn = pymysql.connect(host='localhost', user='radio', password='star11',
                           db='RADIO', charset='utf8')
    try:
        with conn.cursor() as cursor:
            sql = "SELECT username, password FROM user_tbl WHERE username=%s;"
            cursor.execute(sql, (username, ))

            result = cursor.fetchone()
            if result is not None and result[1] == password:
                conn.commit()
                conn.close()
                SESS.set('logined', True)
                SESS.set('username', result[0])
                redirect("/index.py")
                sys.exit()
        conn.commit()
    finally:
        conn.close()
    redirect("/login.py")


if __name__ == '__main__':
    if SESS.get('logined'):
        redirect('index.py')
        exit()

    if METHOD == 'GET':
        render_form('Sign in')
    elif METHOD == 'POST':
        handler_post()
