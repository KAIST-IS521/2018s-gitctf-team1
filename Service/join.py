#!/usr/bin/python
from common import *
from RSA import enc
import pymysql

def handler_post():
    _POST = parse_str(POST)

    if 'username' not in _POST or 'password' not in _POST:
        redirect('/join.py')
        exit()

    username = _POST['username']
    password = enc(_POST['password'])

    conn = pymysql.connect(host='localhost', user='root', password='root', db='RADIO',charset='utf8')
    try:
        with conn.cursor() as cursor:
            # check duplicate username
            sql = "SELECT 1 FROM user_tbl WHERE username=%s;"
            cursor.execute(sql, (username, ))

            result = cursor.fetchone()
        if result is not None:
            redirect('/join.py')
            exit()

        sql = "INSERT INTO user_tbl (username, password) VALUES (%s, %s);"
        cursor.execute(sql, (username, password))
        conn.commit()
    finally:
        conn.close()
    redirect('/login.py')


if __name__ == '__main__':
    if SESS.get('logined', False):
        redirect('index.py')
        exit()

    if METHOD == 'GET':
        form_tpl('Sign up')
    elif METHOD == 'POST':
        handler_post()
