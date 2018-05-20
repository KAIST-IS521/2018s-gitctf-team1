#!/usr/bin/python
import sys
from datetime import datetime
from collections import OrderedDict
import sys
import six
import urllib
import os

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

def parse(query_string, unquote=True, normalized=False, encoding='utf-8'):
  mydict = {}
  plist = []
  if query_string == "":
    return mydict

  if type(query_string) == bytes:
    query_string = query_string.decode()

  for element in query_string.split("&"):
    try:
      if unquote:
        (var, val) = element.split("=")
        if sys.version_info[0] == 2:
          var = var.encode('ascii')
          val = val.encode('ascii')
        var = urllib.unquote_plus(var)
        val = urllib.unquote_plus(val)
      else:
        (var, val) = element.split("=")
    except ValueError:
      raise MalformedQueryStringError
    if encoding:
      var = var.decode(encoding)
      val = val.decode(encoding)
    plist.append(parser_helper(var, val))
  for di in plist:
    (k, v) = di.popitem()
    tempdict = mydict
    while k in tempdict and type(v) is dict:
      tempdict = tempdict[k]
      (k, v) = v.popitem()
    if k in tempdict and type(tempdict[k]).__name__ == 'list':
      tempdict[k].append(v)
    elif k in tempdict:
      tempdict[k] = [tempdict[k], v]
    else:
      tempdict[k] = v

  if normalized == True:
    return _normalize(mydict)
  return mydict


def _normalize(d):
  newd = {}
  if isinstance(d, dict) == False:
    return d
  for k, v in six.iteritems(d):
    if isinstance(v, dict):
      first_key = next(iter(six.viewkeys(v)))
      if isinstance(first_key, int):
        temp_new = []
        for k1, v1 in v.items():
          temp_new.append(_normalize(v1))
        newd[k] = temp_new
      elif first_key == '':
        newd[k] = v.values()[0]
      else:
        newd[k] = _normalize(v)
    else:
      newd[k] = v
  return newd

def parse_str(query_string, unquote=True):
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
    return mydict
  for element in query_string.split("&"):
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
  return os.environ[key]

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
  headers['Location'] = url
  headers['Date'] = datetime.now().strftime("%a, %d %b %Y %H:%M:%S %Z")
  headers['Content-Length'] = 0
  headers['Connection'] = 'Close'

  headers_str = "HTTP/1.1 302 Found\r\n"
  for i in headers:
    headers_str += "%s: %s\r\n" % (i, headers[i])
  headers_str += "\r\n"
  sys.stdout.write(headers_str)
