
import pymysql
from RSA import enc

username = 'admin'
password = open('/var/ctf/flag').read().strip()
password = enc(str(password))

conn = pymysql.connect(host='localhost', user='radio', password='star11',
                       db='RADIO', charset='utf8')
try:
    with conn.cursor() as cursor:
        sql = "INSERT INTO user_tbl (username, password) VALUES (%s, %s);"
        cursor.execute(sql, (username, password))

    conn.commit()
finally:
    conn.close()
