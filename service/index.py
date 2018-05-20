#!/usr/bin/python
from common import *

_GET = parse_str(get_query_string())

try:
  test = _GET['test']
except:
  test = ''

output = 'HELL oWolrd!' + str(test)

headers = {}
headers['Content-Type'] = "text/html"
print_ok(headers=headers, body=output)
