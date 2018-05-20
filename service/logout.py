#!/usr/bin/python
from common import *

if __name__ == '__main__':
  sess = Session()
  sess.delete('logined')
  sess.delete('username')
  redirect('/index.py')
