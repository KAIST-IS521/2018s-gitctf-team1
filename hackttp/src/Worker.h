#pragma once

#include "Request.h"
#include "Response.h"
#include "DataHandler.h"

#include <sys/socket.h>
#include <unistd.h>
#include <string>

#define HTTP_REQUEST_LENGTH 8192

class Worker {
private:
  int socket_fd;
  void send_msg(std::string msg);
public:
  Worker(int fd) : socket_fd(fd) { };
  ~Worker() { };
  void handle_request(std::string root);

  class Exception: public BaseException {
  public:
    Exception(std::string msg = "Unknown worker exception") {
      this->reason = msg;
    }
  };
};
