#pragma once

#include "DataHandler.h"
#include <string>
#include <map>

// HTTP STATUS CODES
#define HTTP_OK                 200
#define HTTP_BAD_REQUEST        400
#define HTTP_FORBIDDEN          403
#define HTTP_NOT_FOUND          404
#define HTTP_UNSUP_MEDIA_TYPE   415
#define HTTP_INTERNAL_SRV_ERROR 500
#define HTTP_NOT_IMPLEMENTED    501

class Response {
public:
  Response(int code, DataHandler::Resource rsrc);
  std::string render();
private:
  int status_code;
  std::string status;
  int length;
  std::map<std::string, std::string> headers;
  std::string body;
};
