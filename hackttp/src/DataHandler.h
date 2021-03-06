#pragma once

#include "Request.h"
#include "Exceptions.h"
#include <cstring>
#include <cerrno>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>

class Response;

class DataHandler {
public:
  struct Resource {
    std::string type;
    std::string data;
  };

  DataHandler();
  virtual ~DataHandler();
  Response *read_Resource(Request req, std::string path);

  // Exception base
  class Exception: public BaseException {
  public:
    Exception(std::string msg = "Unknown data handler exception") {
      this->reason = msg;
    }
  };

  class FileNotFound: public Exception {
  public:
    FileNotFound(std::string msg = "Could not find requested file") {
      this->reason = msg;
    }
  };
  class Unsupported: public Exception {
  public:
    Unsupported(std::string msg = "Unsupported file found") {
      this->reason = msg;
    }
  };
};
