#!/usr/bin/python
from common import *

def handler_get():
  output  = ''
  output += '<!DOCTYPE html>'
  output += '<html lang="en">'
  output += '<head>'
  output += '<meta charset="utf-8" />'
  output += '<title>Login in</title>'
  output += '<link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.1.1/css/bootstrap.min.css" />'
  output += '<style>body,html{height:100%}body{display:-ms-flexbox;display:flex;-ms-flex-align:center;align-items:center;padding-top:40px;padding-bottom:40px;background-color:#f5f5f5}.form-signin{width:100%;max-width:330px;padding:15px;margin:auto}.form-signin .checkbox{font-weight:400}.form-signin .form-control{position:relative;box-sizing:border-box;height:auto;padding:10px;font-size:16px}.form-signin .form-control:focus{z-index:2}.form-signin input[type=email]{margin-bottom:-1px;border-bottom-right-radius:0;border-bottom-left-radius:0}.form-signin input[type=password]{margin-bottom:10px;border-top-left-radius:0;border-top-right-radius:0}</style>'
  output += '</head>'
  output += '<body class="text-center">'
  output += '<form class="form-signin" method="post" action="">'
  output += '<h1 class="h3 mb-3 font-weight-normal">Sign in</h1>'
  output += '<label for="inputUsername" class="sr-only">Username</label>'
  output += '<input type="text" id="inputUsername" name="username" class="form-control" placeholder="Username" required autofocus>'
  output += '<label for="inputPassword" class="sr-only">Password</label>'
  output += '<input type="password" id="inputPassword" name="password" class="form-control" placeholder="Password" required>'
  output += '</div>'
  output += '<button class="btn btn-lg btn-primary btn-block" type="submit">Sign in</button>'
  output += '<p class="mt-5 mb-3 text-muted">&copy; 2018 IS521-GITCTF-TEAM1</p>'
  output += '</form>'
  output += '</body>'
  output += '</html>'

  headers = {}
  headers['Content-Type'] = "text/html"
  print_ok(headers=headers, body=output)

def handler_post():
  _POST = parse_str(raw_input())
  headers = {}
  headers['Content-Type'] = "text/html"
  print_ok(headers=headers, body=str(_POST))

if __name__ == '__main__':
  if get_method() == 'GET':
    handler_get()
  elif get_method() == 'POST':
    handler_post()
