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
            sql = "SELECT username, password FROM user_tbl WHERE username=%s;"
            cursor.execute(sql, (username, ))

            result = cursor.fetchone()
            if result is not None and result[1] == password:
                sess.set('logined', True)
                sess.set('username', result[0])
                redirect("/index.py")
                exit()
        conn.commit()
    finally:
        conn.close()
    redirect("/login.py")


if __name__ == '__main__':
    if SESS.get('logined'):
        redirect('index.py')
        exit()

    if METHOD == 'GET':
        form_tpl('Sign in')
    elif METHOD == 'POST':
        handler_post()
