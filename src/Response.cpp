#include "Response.h"
#include "DataHandler.h"

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
  //length = ;
  //body
}

std::string Response::render() {
  std::string output;

  output += "HTTP/1.1 " + std::to_string(code) + " " + status + "\r\n";
  for (auto it = headers.begin(); it != headers.end(); ++i) {
    output += it->first + ": " + it->second + "\r\n";
  }
  output += "\r\n";
  output += body;

  returnd output;
}
