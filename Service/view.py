#!/usr/bin/python
from common import SESS, POST, METHOD
from common import redirect, print_ok, parse_str, get_base
from common import render_base, get_template
from hashlib import md5

import os
import pymysql
import subprocess


def md5hash(st):
    return md5(st).hexdigest()


def hexdump(data):
    fname = os.path.join(get_base(), 'tmp', md5hash(data))
    with open(fname, 'wb') as f:
        f.write(data)
    out = subprocess.check_output('cat {} | xxd'.format(fname), shell=True)
    os.unlink(fname)
    return out


def view():
    username = SESS.get('username')
    if username == '':
        redirect('/index.py#invalid+SESSion')
        exit()

    conn = pymysql.connect(host='localhost', user='radio', password='star11',
                           db='RADIO', charset='utf8')
    try:
        sql = "SELECT idx, data FROM save_tbl WHERE username=%s" \
              " ORDER BY idx DESC LIMIT 10;"

        with conn.cursor() as cursor:
            cursor.execute(sql, (username, ))
            result = cursor.fetchall()

        data = ''.join('''<tr onclick="open_modal(%d)">
                          <td>%d</td><td>%s</td></tr>''' % (i, i, md5hash(j))
                       for i, j in result)
        conn.commit()
    finally:
        conn.close()
    render_base(get_template('views.tpl') % data)


def fetch():
    username = SESS.get('username')
    if username == '':
        redirect('/index.py#invalid+session')
        exit()

    _POST = parse_str(POST)
    output = ''
    result = None
    if 'idx' in _POST:
        idx = int(_POST['idx'])
        conn = pymysql.connect(host='localhost', user='radio', password='star11',
                               db='RADIO', charset='utf8')
        try:
            with conn.cursor() as cursor:
                sql = "SELECT username, data FROM save_tbl WHERE idx=%s;"
                cursor.execute(sql, (idx, ))
                result = cursor.fetchone()
            output = hexdump(result[1])
        except Exception:
            pass
        finally:
            conn.commit()
            conn.close()

    if result is None:
        redirect('/index.py#not+found')
        exit()

    if result[0] != username:
        redirect('/index.py#invalid+username')
        exit()

    headers = {'Content-Type': 'text/plain'}

    tmp = SESS.get_setcookie()
    if tmp is not None:
        headers['Set-Cookie'] = tmp
    print_ok(headers=headers, body=output)


if __name__ == '__main__':
    if not SESS.get('logined'):
        redirect('index.py')
        exit()
    if METHOD == 'GET':
        view()
    elif METHOD == 'POST':
        fetch()
