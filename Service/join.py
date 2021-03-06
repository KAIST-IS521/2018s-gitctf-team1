#!/usr/bin/python
from common import *
from RSA import enc
import string
import pymysql


def check_valid(x):
    return x in string.uppercase + string.lowercase + string.digits


def handler_post():
    _POST = parse_str(POST)

    if 'username' not in _POST or 'password' not in _POST:
        redirect('/join.py')
        exit()

    username = str(_POST['username'])
    if not all(check_valid(c) for c in username):
        redirect('/login.py')
        exit(0)
    password = enc(str(_POST['password']))

    conn = pymysql.connect(host='localhost', user='radio', password='star11',
                           db='RADIO', charset='utf8')
    try:
        with conn.cursor() as cursor:
            # check duplicate username
            sql = "SELECT 1 FROM user_tbl WHERE username=%s;"
            cursor.execute(sql, (username, ))

            result = cursor.fetchone()
        if result is not None:
            conn.commit()
            conn.close()
            redirect('/join.py')
            exit()

        with conn.cursor() as cursor:
            sql = "INSERT INTO user_tbl (username, password) VALUES (%s, %s);"
            cursor.execute(sql, (username, password))

        conn.commit()
    finally:
        conn.close()
    redirect('/login.py')


if __name__ == '__main__':
    if SESS.get('logined'):
        redirect('index.py')
        exit()

    if METHOD == 'GET':
        form_tpl('Sign up')
    elif METHOD == 'POST':
        handler_post()
