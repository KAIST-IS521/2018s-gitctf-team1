#!/usr/bin/python
from common import *
import pymysql

def handler_get(sess):
  output  = ''
  output += '<!DOCTYPE html>'
  output += '<html lang="en">'
  output += '<head>'
  output += '<meta charset="utf-8" />'
  output += '<title>Sign up</title>'
  output += '<link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.1.1/css/bootstrap.min.css" />'
  output += '<style>body,html{height:100%}body{display:-ms-flexbox;display:flex;-ms-flex-align:center;align-items:center;padding-top:40px;padding-bottom:40px;background-color:#f5f5f5}.form-signin{width:100%;max-width:330px;padding:15px;margin:auto}.form-signin .checkbox{font-weight:400}.form-signin .form-control{position:relative;box-sizing:border-box;height:auto;padding:10px;font-size:16px}.form-signin .form-control:focus{z-index:2}.form-signin input[type=email]{margin-bottom:-1px;border-bottom-right-radius:0;border-bottom-left-radius:0}.form-signin input[type=password]{margin-bottom:10px;border-top-left-radius:0;border-top-right-radius:0}</style>'
  output += '</head>'
  output += '<body class="text-center">'
  output += '<form class="form-signin" method="post" action="">'
  output += '<h1 class="h3 mb-3 font-weight-normal">Sign up</h1>'
  output += '<label for="inputUsername" class="sr-only">Username</label>'
  output += '<input type="text" id="inputUsername" name="username" class="form-control" placeholder="Username" required autofocus>'
  output += '<label for="inputPassword" class="sr-only">Password</label>'
  output += '<input type="password" id="inputPassword" name="password" class="form-control" placeholder="Password" required>'
  output += '</div>'
  output += '<button class="btn btn-lg btn-primary btn-block" type="submit">Sign up</button>'
  output += '<p class="mt-5 mb-3 text-muted">&copy; 2018 IS521-GITCTF-TEAM1</p>'
  output += '</form>'
  output += '</body>'
  output += '</html>'


  headers = {}
  headers['Content-Type'] = "text/html"

  tmp = sess.get_setcookie()
  if tmp != None:
    headers['Set-Cookie'] = tmp

  print_ok(headers=headers, body=output)

def handler_post(sess):
  # get username from data to check exist or not
  # if not exist process or Redirect index.py

  _POST = parse_str(raw_input())

  #password = rsaencrpt(password) # TODO: HanSungho

  conn = pymysql.connect(host='localhost', user='root', password='root', db='User',charset='utf8')
  try:
    with conn.cursor() as cursor:
      sql = "INSERT INTO user_tbl (username, password) VALUES (%s, %s);"
      cursor.execute(sql, (_POST['username'], _POST['password']))
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
