#!/usr/bin/python
import sys
import urllib
import os
import yaml
import re

from string import ascii_letters, digits
from random import choice
from datetime import datetime
from collections import OrderedDict


def get_base():
    return os.path.abspath(os.path.dirname(__file__))


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
    return start != -1


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
    def recurse(pair, out=None, indent=0):
        if isinstance(pair, dict):
            (k, v) = pair.popitem()
            if out is None:
                if isinstance(k, int) or k == '':
                    out = []
                else:
                    out = OrderedDict()
            if isinstance(out, list) and (k == ''):
                out.append(recurse(v, indent=indent+1))
            elif isinstance(out, list) and (k != ''):
                out = dict(enumerate(out))  # merge list with dict
                out[k] = recurse(v, out.get(k), indent=indent+1)
            elif isinstance(out, dict) and k == '':
                numeric_indices = [x for x in out.keys() if isinstance(x, int)]
                out[1+max(numeric_indices) if numeric_indices else 0] = \
                    recurse(v, out.get(k))
            elif isinstance(out, dict):
                out[k] = recurse(v, out.get(k), indent=indent+1)
            else:
                # overwriting scalar out by dict or list
                return recurse({k: v}, indent=indent+1)
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

    out = []
    for pair in plist:
        out = recurse(pair, out)
    return out


def get_environ(key):
    return os.environ.get(key, "")


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
    headers = {'Location': url,
               'Date': datetime.now().strftime("%a, %d %b %Y %H:%M:%S %Z"),
               'Content-Length': 0,
               'Connection': 'Close'}

    headers_str = "HTTP/1.1 302 Found\r\n"
    for i in headers:
        headers_str += "%s: %s\r\n" % (i, headers[i])
    headers_str += "\r\n"
    sys.stdout.write(headers_str)


class Session:
    def __init__(self):
        self.keyword = 'gitctfsessid'
        self.session_dir = os.path.join(get_base(), 'tmp')
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

        with open(self.path, 'rb') as fp:
            self.data = yaml.load(fp)

    def get_setcookie(self):
        if self.new_flag:
            return "%s=%s; HttpOnly" % (self.keyword, self.identifier)
        else:
            return None

    def save_file(self):
        with open(self.path, 'wb') as fp:
            yaml.dump(self.data, fp)

    def get(self, key):
        return self.data.get(key, None)

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


def get_template(t):
    with open(os.path.join(get_base(), 'templates', t)) as f:
        return f.read()


def render_base(main):
    template = get_template('base.tpl')
    if SESS.get('logined'):
        output = template % (get_template('logined.tpl'), main)
    else:
        output = template % (get_template('notlogined.tpl'), main)

    headers = {'Content-Type': 'text/html'}
    tmp = SESS.get_setcookie()
    if tmp is not None:
        headers['Set-Cookie'] = tmp
    print_ok(headers=headers, body=output)


def render_form(keyword):
    template = get_template('form.tpl')
    output = template % (keyword, keyword, keyword)

    headers = {'Content-Type': 'text/html'}
    tmp = SESS.get_setcookie()
    if tmp is not None:
        headers['Set-Cookie'] = tmp

    print_ok(headers=headers, body=output)
