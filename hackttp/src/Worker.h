#pragma once

#include "Request.h"
#include "Response.h"
#include "DataHandler.h"

#include <sys/socket.h>
#include <unistd.h>

#define HTTP_REQUEST_LENGTH 8192

class Worker {
private:
  int socket_fd;
  void send_msg(char * msg, long size);
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
