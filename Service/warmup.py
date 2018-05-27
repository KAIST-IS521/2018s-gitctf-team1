#!/usr/bin/python
from common import *
from subprocess import PIPE, Popen

def exec_cmd(cmd):
    p = Popen(args=cmd, stdout=PIPE, stderr=PIPE, shell=True)
    return p.communicate()[0]

def handler_get():
    body = '<form method="post" action="">'
    body += '<input type="text" name="dir" placeholder="Input base64-encoded directory name you want to see"/>'
    body += '<input type="submit" value="Submit"/>'
    body += '</form>'
    main_tpl(body)

def handler_post():
    _POST = parse_str(POST)

    if 'dir' not in _POST:
        redirect('/warmup.py#invalid+input')
        exit()

    dirname = str(_POST['dir']).decode('base64').strip()
    data = exec_cmd('ls %s' % dirname)

    if data == '':
        redirect('/warmup.py#not+found')
        exit()

    body = '<pre>%s</pre>' % data
    main_tpl(body)

if __name__ == '__main__':
    if not SESS.get('logined'):
        redirect('index.py')
        exit()
    if METHOD == 'GET':
        handler_get()
    elif METHOD == 'POST':
        handler_post()
