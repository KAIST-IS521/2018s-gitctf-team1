import pymysql

def init_db(conn):
  try:
	  with conn.cursor() as cursor:
		  sql = 'CREATE DATABASE User'
		  cursor.execute(sql)
	  conn.commit()
  finally:
	  conn.close()

def init_table(conn):
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
  conn = pymysql.connect(host='localhost', user='root', password='root', db='User', charset='utf8')
  init_db(conn)
  init_table(conn)
