#include "Response.h"
#include "DataHandler.h"

#include <iostream>

Response::Response(int code, DataHandler::Resource rsrc) {
  status_code = code;

  switch(status_code) {
    case HTTP_OK:                 status = "OK"; break;
    case HTTP_BAD_REQUEST:        status = "Bad Request"; break;
    case HTTP_FORBIDDEN:          status = "Forbidden"; break;
    case HTTP_NOT_FOUND:          status = "Not Found"; break;
    case HTTP_UNSUP_MEDIA_TYPE:   status = "Unsupported Media Type"; break;
    case HTTP_INTERNAL_SRV_ERROR: status = "Internal Server Error"; break;
    case HTTP_NOT_IMPLEMENTED:    status = "Not Implemented"; break;
    default:                      status = "Unknown error";
  }
  
  std::string &data = rsrc.data;
  if (data.compare(0, 6, "HTTP/1") == 0) {
    
  } else { // maybe no header
    length = data.size();
    headers.insert(std::make_pair("Server", "IS521-2018s-GITCTF-TEAM1"));
    headers.insert(std::make_pair("Content-Type", rsrc.type == ""? "text/plain": rsrc.type));
    headers.insert(std::make_pair("Content-Length", std::to_string(data.size())));
    headers.insert(std::make_pair("Connection", "Close"));
    body = data;
  }
}

std::string Response::render() {
  std::string output;

  output += "HTTP/1.1 " + std::to_string(status_code) + " " + status + "\r\n";
  for (auto it = headers.begin(); it != headers.end(); ++it) {
    output += it->first + ": " + it->second + "\r\n";
  }
  output += "\r\n";
  output += body;

  return output;
}
