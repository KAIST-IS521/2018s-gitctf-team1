#pragma once

#include <string>
#include <map>

class Request {
private:
  bool valid;
  std::string method;
  std::string uri;
  std::string http_version;
 
  std::map<std::string, std::string> header;
  std::map<std::string, std::string> get; // GET_PARAMETERS
  std::map<std::string, std::string> post; // POST_PARAMETERS
  std::map<std::string, std::string> cookie; // COOKIES

  void fetch_headers(std::string req_str);
  void fetch_cookies(std::string req_str);
  void fetch_queries(std::string req_str);
  void fetch_forms(std::string req_str);
  std::string urldecode(std::string data);
  std::string getParameters(std::map<std::string, std::string> target);

public:
  Request(std::string req_str);
  std::string getHeader(std::string key);
  std::string getQueryString();
  std::string getPostData();
  virtual ~Request();
};
