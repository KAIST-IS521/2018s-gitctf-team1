#!/usr/bin/python
from common import *
from hashlib import md5
import pymysql


def hexdump(src, length=16):
    FILTER = ''.join([(len(repr(chr(x))) == 3) and chr(x) or '.' for x in range(256)])
    ines = []
    for c in xrange(0, len(src), length):
        chars = src[c:c+length]
        hex = ' '.join(["%02x" % ord(x) for x in chars])
        printable = ''.join(["%s" % ((ord(x) <= 127 and FILTER[ord(x)]) or '.') for x in chars])
        lines.append("%04x  %-*s  %s\n" % (c, length*3, hex, printable))
    return ''.join(lines)


def view():
    username = SESS.get('username')
    if username == '':
        redirect('/index.py#invalid+SESSion')
        exit()

    body = ''

    body += '<table class="table">'
    body += '<thead><tr><th>IDX</th><th>HASH</th></tr></thead><tbody>'
    conn = pymysql.connect(host='localhost', user='root', password='root', db='RADIO',charset='utf8')
    try:
        with conn.cursor() as cursor:
            sql = "SELECT idx, data FROM save_tbl WHERE username=%s ORDER BY idx DESC LIMIT 10;"
            cursor.execute(sql, (username, ))
            result = cursor.fetchall()
        for i in result:
            body += '<tr onclick="open_modal(%d)"><td>%d</td><td>%s</td></tr>' % (i[0], i[0], md5(i[1]).hexdigest())
        conn.commit()
    finally:
        conn.close()

    body += '</tbody></table>'
    body += '<div class="modal" tabindex="-1" role="dialog" id="modall">'
    body +=   '<div class="modal-dialog" role="document" style="max-width: 600px; text-align: left">'
    body +=     '<div class="modal-content">'
    body +=       '<div class="modal-header">'
    body +=         '<button type="button" class="close" data-dismiss="modal" aria-label="Close">'
    body +=           '<span aria-hidden="true">&times;</span>'
    body +=         '</button>'
    body +=       '</div>'
    body +=       '<div class="modal-body">'
    body +=         '<xmp></xmp>'
    body +=       '</div>'
    body +=       '<div class="modal-footer">'
    body +=         '<button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>'
    body +=       '</div>'
    body +=     '</div>'
    body +=   '</div>'
    body += '</div>'
    body += '<script src="https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js"></script>'
    body += '<script src="https://stackpath.bootstrapcdn.com/bootstrap/4.1.1/js/bootstrap.min.js"></script>'
    body += '<script>function open_modal(idx) { $.post("view.py", "idx=" + idx, function(d){$("xmp").text(d) }); $(\'#modall\').modal(\'show\') }</script>'

    main_tpl(body)

def fetch():
    username = SESS.get('username')
    if username == '':
        redirect('/index.py#invalid+session')
        exit()

    _POST = parse_str(POST)
    output = ''
    if 'idx' in _POST:
        idx = int(_POST['idx'])

        conn = pymysql.connect(host='localhost', user='root', password='root', db='RADIO',charset='utf8')
        try:
            with conn.cursor() as cursor:
                sql = "SELECT username, data FROM save_tbl WHERE idx=%s;"
                cursor.execute(sql, (idx, ))
                result = cursor.fetchone()
            output = hexdump(result[1])
            conn.commit()
        finally:
            conn.close()

    if result[0] != username:
        redirect('/index.py#invalid+username')
        exit()

    headers = {}
    headers['Content-Type'] = "text/plain"

    tmp = SESS.get_setcookie()
    if tmp != None:
        headers['Set-Cookie'] = tmp
    print_ok(headers=headers, body=output)

if __name__ == '__main__':
    if not SESS.get('logined'):
        redirect('index.py')
        exit()
    if METHOD = 'GET':
        view()
    elif METHOD == 'POST':
        fetch()
