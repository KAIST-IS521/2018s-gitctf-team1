#!/usr/bin/python
from common import *

if __name__ == '__main__':
    SESS.delete('logined')
    SESS.delete('username')
    redirect('/index.py')
