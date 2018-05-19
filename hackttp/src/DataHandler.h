#pragma once
#include "Request.h"
#include "Response.h"
#include "Exceptions.h"
#include <cstring>
#include <cerrno>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>

class DataHandler {
private:
  std::string get_working_path();
  bool verify_path(std::string path);

public:
  struct Resource {
    std::string type;
    std::string data;
  };

  DataHandler(std::string client);
  virtual ~DataHandler();
  Resource read_Resource(std::string path);
  Resource read_Resource(Request  req);  

  // Exception base
  class Exception: public BaseException {
  public:
    Exception(std::string msg = "Unknown data handler exception") {
      this->reason = msg;
    }
  };

  class Exec {
  public:
    Exec();
    ~Exec();
    Resource run_command(std::string args[]);
    Resource run_command(std::string args[], Request req);
		//Resource run_command(std::string args[]);

    class PermissionDenied: public Exception {
    public:
      PermissionDenied(std::string msg = "Permission denied while trying to execute command") {
        this->reason = msg;
      }
    };
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

