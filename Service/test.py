#!/usr/bin/python
import sys
import os
data = '<ul>\n'
t = os.environ
for x in t:
  data += '\t<li>%s:%s</li>\n' % (x, t[x])
data += '</ul>'

sys.stdout.write("HTTP/1.1 200 OK\r\n")
#sys.stdout.write("Date: Mon, 27 Jul 2009 12:28:53 GMT\r\n"
sys.stdout.write("Server: Apache/2.2.14 (Win32)\r\n")
#sys.stdout.write("Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n"
sys.stdout.write("Content-Length: %d\r\n" % len(data))
sys.stdout.write("Content-Type: text/html\r\n")
sys.stdout.write("Connection: Close\r\n")
sys.stdout.write("\r\n")
sys.stdout.write(data)
