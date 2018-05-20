import pymysql
DB_HOST = '127.0.0.1'
DB_USER = 'root'
DB_PASS = 'root'
DB_NAME = 'RADIO'

def init_db():
  conn = pymysql.connect(host=DB_HOST, user=DB_USER, password=DB_PASS, charset='utf8')
  try:
    with conn.cursor() as cursor:
      sql = 'CREATE DATABASE %s' % (DB_NAME)
      cursor.execute(sql)
    conn.commit()
  finally:
    conn.close()

def init_table():
  conn = pymysql.connect(host=DB_HOST, user=DB_USER, password=DB_PASS, db=DB_NAME, charset='utf8')
  try:
    with conn.cursor() as cursor:
      sql = '''
      CREATE TABLE IF NOT EXISTS `user_tbl` (
          `idx`               int(11) NOT NULL AUTO_INCREMENT,
          `username`          text not null,
          `password`          text not null,
          PRIMARY KEY (`idx`)
      ) ENGINE=MyISAM DEFAULT CHARSET=utf8;
      '''
      cursor.execute(sql)
    conn.commit()
  finally:
      conn.close()

if __name__ == '__main__':
  init_db()
  init_table()
