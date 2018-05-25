#!/usr/bin/python
from common import SESS
from common import redirect


if __name__ == '__main__':
    SESS.delete('logined')
    SESS.delete('username')
    redirect('/index.py')
