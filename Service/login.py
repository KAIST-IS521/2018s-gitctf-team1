#!/usr/bin/python
from common import *
from RSA import enc
import pymysql

def handler_get(sess):
  form_tpl('Sign in', sess)

def handler_post(sess):
  _POST = parse_str(raw_input())

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
  sess = Session()
  if sess.get('logined') == True:
    redirect('index.py')
    exit()

  if get_method() == 'GET':
    handler_get(sess)
  elif get_method() == 'POST':
    handler_post(sess)
