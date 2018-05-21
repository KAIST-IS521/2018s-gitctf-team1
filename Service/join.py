#!/usr/bin/python
from common import *
from RSA import enc
import pymysql

def handler_get(sess):
  form_tpl('Sign up', sess)

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
  sess = Session()
  if sess.get('logined') == True:
    redirect('index.py')
    exit()

  if get_method() == 'GET':
    handler_get(sess)
  elif get_method() == 'POST':
    handler_post(sess)
