#!/usr/bin/python
import sys
import urllib
import os
import json
import re

from string import ascii_letters, digits
from random import choice
from datetime import datetime
from collections import OrderedDict


def random_str(l):
  return ''.join(choice(ascii_letters + digits) for n in xrange(l))


def has_variable_name(s):
  if s.find("[") > 0:
    return True


def more_than_one_index(s, brackets=2):
  start = 0
  brackets_num = 0
  while start != -1 and brackets_num < brackets:
    start = s.find("[", start)
    if start == -1:
      break
    start = s.find("]", start)
    brackets_num += 1
  if start != -1:
    return True
  return False


def get_key(s):
  start = s.find("[")
  end = s.find("]")
  if start == -1 or end == -1:
    return None
  if s[start + 1] == "'":
    start += 1
  if s[end - 1] == "'":
    end -= 1
  return s[start + 1:end]  # without brackets


def is_number(s):
  if len(s) > 0 and s[0] in ('-', '+'):
    return s[1:].isdigit()
  return s.isdigit()


class MalformedQueryStringError(Exception):
  pass


def parser_helper(key, val):
  start_bracket = key.find("[")
  end_bracket = key.find("]")
  pdict = {}
  if has_variable_name(key):  # var['key'][3]
    pdict[key[:key.find("[")]] = parser_helper(key[start_bracket:], val)
  elif more_than_one_index(key):  # ['key'][3]
    newkey = get_key(key)
    #newkey = int(newkey) if is_number(newkey) else newkey
    pdict[newkey] = parser_helper(key[end_bracket + 1:], val)
  else:  # key = val or ['key']
    newkey = key
    if start_bracket != -1:  # ['key']
      newkey = get_key(key)
      if newkey is None:
        raise MalformedQueryStringError
    newkey = int(newkey) if is_number(newkey) else newkey
    val = int(val) if is_number(val) else val
    pdict[newkey] = val
  return pdict


def parse_str(query_string, unquote=True, delimiter='&'):
  def recurse( pair, out=None, indent=0 ):
    if isinstance(pair, dict):
      (k, v) = pair.popitem()
      if out is None:
        if isinstance(k,int) or k == '':
          out = []
        else:
          out = OrderedDict()
      if isinstance(out,list) and (k == ''):
        out.append( recurse(v,indent=indent+1) )
      elif isinstance(out,list) and (k != ''):
        out = dict(enumerate(out)) # merge list with dict
        out[k] = recurse(v, out.get(k),indent=indent+1)
      elif isinstance(out,dict) and k == '':
        numeric_indices = [k for k in out.keys() if isinstance(k,int)]
        out[1+max(numeric_indices) if numeric_indices else 0] = recurse(v, out.get(k))
      elif isinstance(out,dict):
        out[k] = recurse(v, out.get(k),indent=indent+1)
      else:
        return recurse({k:v},indent=indent+1) # overwriting scalar out by dict or list
      return out
    return pair

  plist = []
  if query_string == "":
    return []
  for element in query_string.split(delimiter):
    try:
      if unquote:
        (var, val) = element.split("=")
        var = urllib.unquote_plus(var)
        val = urllib.unquote_plus(val)
      else:
        (var, val) = element.split("=")
    except ValueError:
      raise MalformedQueryStringError
    plist.append(parser_helper(var, val))


  out = []#OrderedDict()
  for pair in plist:
    out = recurse(pair, out)
  return out


def get_environ(key):
  try:
    return os.environ[key]
  except:
    return ""


def get_method():
  return get_environ('REQUEST_METHOD')


def get_content_length():
  return get_environ('CONTENT_LENGTH')


def get_query_string():
  return get_environ('QUERY_STRING')


def get_cookie():
  return get_environ('COOKIE')


def print_ok(headers={}, body=""):
  headers['Date'] = datetime.now().strftime("%a, %d %b %Y %H:%M:%S %Z")
  headers['Content-Length'] = str(len(body))
  headers['Connection'] = 'Close'

  headers_str = "HTTP/1.1 200 OK\r\n"
  for i in headers:
    headers_str += "%s: %s\r\n" % (i, headers[i])
  headers_str += "\r\n"

  sys.stdout.write(headers_str + body)


def redirect(url):
  headers = {}
  headers['Location'] = url
  headers['Date'] = datetime.now().strftime("%a, %d %b %Y %H:%M:%S %Z")
  headers['Content-Length'] = 0
  headers['Connection'] = 'Close'

  headers_str = "HTTP/1.1 302 Found\r\n"
  for i in headers:
    headers_str += "%s: %s\r\n" % (i, headers[i])
  headers_str += "\r\n"
  sys.stdout.write(headers_str)


class Session:
  def __init__(self):
    self.keyword = 'gitctfsessid'
    self.session_dir = os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + '/' + 'session')
    self.identifier = None
    self.data = {}

    # no such directory
    if not os.path.isdir(self.session_dir):
      os.mkdir(self.session_dir)

    # if no headers
    cookie = parse_str(get_cookie(), delimiter=';')
    self.new_flag = True
    if self.keyword in cookie:
      self.identifier = cookie[self.keyword]
      if re.match("[0-9A-Za-z]{32}", self.identifier):
        self.new_flag = False

    if self.new_flag:
      self.identifier = random_str(32)

    self.path = os.path.realpath(self.session_dir + '/' + self.identifier)

    if self.new_flag or not os.path.exists(self.path):
      self.save_file()

    with open (self.path, 'rb') as fp:
      self.data = json.load(fp)

  def get_setcookie(self):
    if self.new_flag:
      return "%s=%s; HttpOnly" % (self.keyword, self.identifier)
    else:
      return None

  def save_file(self):
    with open(self.path, 'wb') as fp:
      json.dump(self.data, fp)

  def get(self, key):
    if key in self.data:
      return self.data[key]
    else:
      return None

  def set(self, key, val):
    self.data[key] = val
    self.save_file()

  def delete(self, key):
    if key in self.data:
      del self.data[key]
      self.save_file()

METHOD = get_environ('REQUEST_METHOD')
POST = sys.stdin.read(int(get_environ('CONTENT_LENGTH')))
GET = get_environ('QUERY_STRING')
SESS = Session()

def main_tpl(main):
  sess = SESS
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
  output +=     main
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

def form_tpl(keyword):
  sess = SESS
  output  = ''
  output += '<!DOCTYPE html>'
  output += '<html lang="en">'
  output += '<head>'
  output += '<meta charset="utf-8" />'
  output += '<title>%s</title>' % (keyword)
  output += '<link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.1.1/css/bootstrap.min.css" />'
  output += '<style>body,html{height:100%}body{display:-ms-flexbox;display:flex;-ms-flex-align:center;align-items:center;padding-top:40px;padding-bottom:40px;background-color:#f5f5f5}.form-signin{width:100%;max-width:330px;padding:15px;margin:auto}.form-signin .checkbox{font-weight:400}.form-signin .form-control{position:relative;box-sizing:border-box;height:auto;padding:10px;font-size:16px}.form-signin .form-control:focus{z-index:2}.form-signin input[type=email]{margin-bottom:-1px;border-bottom-right-radius:0;border-bottom-left-radius:0}.form-signin input[type=password]{margin-bottom:10px;border-top-left-radius:0;border-top-right-radius:0}</style>'
  output += '</head>'
  output += '<body class="text-center">'
  output += '<form class="form-signin" method="post" action="">'
  output += '<h1 class="h3 mb-3 font-weight-normal">%s</h1>'% (keyword)
  output += '<label for="inputUsername" class="sr-only">Username</label>'
  output += '<input type="text" id="inputUsername" name="username" class="form-control" placeholder="Username" required autofocus>'
  output += '<label for="inputPassword" class="sr-only">Password</label>'
  output += '<input type="password" id="inputPassword" name="password" class="form-control" placeholder="Password" required>'
  output += '</div>'
  output += '<button class="btn btn-lg btn-primary btn-block" type="submit">%s</button>' % (keyword)
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
