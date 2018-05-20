#!/usr/bin/python
from common import *

def index():
  sess = Session()
  logined = sess.get('logined') == True

  output  = ''
  output += '<!DOCTYPE html>'
  output += '<html lang="en">'
  output += '<head>'
  output +=   '<meta charset="utf-8" />'
  output +=   '<title>Ra.D.io</title>'
  output +=   '<link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.1.1/css/bootstrap.min.css" />'
  output +=   '<style>body{padding-top:5rem}.starter-template{padding:3rem 1.5rem;text-align:center}</style>'
  output += '</head>'
  output += '<body>'
  output += '<nav class="navbar navbar-expand-md navbar-dark bg-dark fixed-top">'
  output +=   '<div class="container">'
  output +=     '<div class="collapse navbar-collapse" id="navbarsExampleDefault">'
  output +=       '<ul class="navbar-nav mr-auto">'
  output +=         '<li class="nav-item"><a class="nav-link" href="index.py">Main</a></li>'
  if logined:
    output +=         '<li class="nav-item"><a class="nav-link" href="request.py">Request</a></li>'
    output +=         '<li class="nav-item"><a class="nav-link" href="view.py">View</a></li>'
    output +=         '<li class="nav-item"><a class="nav-link" href="logout.py">Logout</a></li>'
  else:
    output +=         '<li class="nav-item"><a class="nav-link" href="login.py">Login</a></li>'
    output +=         '<li class="nav-item"><a class="nav-link" href="join.py">Join</a></li>'
  output +=       '</ul>'
  output +=     '</div>'
  output +=   '</div>'
  output += '</nav>'
  output += '<main role="main" class="container">'
  output +=   '<div class="starter-template">'
  output +=     '<h1>Ra.D.io</h1>'
  output +=     '<hr />'
  output +=     '<iframe width="560" height="315" src="https://www.youtube-nocookie.com/embed/Y_sW-Oc9BJY?rel=0" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>'
  output +=     '<hr />'
  output +=     '<p class="mt-5 mb-3 text-muted">&copy; 2018 IS521-GITCTF-TEAM1</p>'
  output +=   '</div>'
  output += '</main>'#<!-- /.container -->
  output += '</body>'
  output += '</html>'

  headers = {}
  headers['Content-Type'] = "text/html"

  tmp = sess.get_setcookie()
  if tmp != None:
    headers['Set-Cookie'] = tmp

  print_ok(headers=headers, body=output)

if __name__ == '__main__':
  index()
