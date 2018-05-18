#include <iostream>
#include "Request.h"

int main(int argc, char *argv[]) {
  std::string test = "GET /foobar HTTP/1.1\r\n";
  test += "Host: pagez.kr\r\n";
  test += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n";
  test += "Accept-Encoding: gzip, deflate, br\r\n";
  test += "Accept-Language: en-US,en;q=0.9\r\n";
  test += "Cache-Control: no-cache\r\n";
  test += "Cookie: __cfduid=d9c2792fa4be464f906fdef4e28055c5b1526533667\r\n";
  test += "Pragma: no-cache\r\n";
  test += "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.139 Safari/537.36\r\n";

  Request req(test);

  return 0;
}
