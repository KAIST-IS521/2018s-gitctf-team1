#include "Router.h"
#include "Worker.h"

#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

Router::Router(int qsize, int port) {
  this->queue_size = qsize;
  this->port = port;
  this->init_socket();
}

Router::~Router() {
  close(this->listening_socket_fd);
}

void Router::watch() {
  listen(this->listening_socket_fd, this->queue_size);
  while (!isSigintReceived) {
    struct sockaddr_storage client;
    socklen_t addr_size = sizeof client;
    int client_fd;

    client_fd = accept(this->listening_socket_fd, (struct sockaddr *) &client, &addr_size);
    if (client_fd < 0) {
      if(errno == EINTR) break;
      else {
        char *err = ::strerror(errno);
        close(this->listening_socket_fd);
        throw Router::Exception("Error when accepting connection: " + std::string(err ? err : "unknown error"));
      }
    }

    pid_t pid = fork();
    if (pid < 0) { throw Router::Exception("fork error"); }
    else if (pid == 0) { // child
      close(this->listening_socket_fd); // don't need server fd

      struct sockaddr_in *sin = (struct sockaddr_in *)&client;
      char client_addr[INET6_ADDRSTRLEN];
      inet_ntop(AF_INET, &sin->sin_addr, client_addr, sizeof client_addr);

      fprintf(stdout, "%s:%d\n", client_addr, sin->sin_port);

      Worker w((int)client_fd); // call worker
      w.handle_request();
    } else {
      close(client_fd); // parent don't need clind fd anymore
    }
  }
}

void Router::init_socket() {
  listening_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listening_socket_fd < 0) {
    char * err = ::strerror(errno);
    throw Router::Exception("socket: " + std::string(err? err: "unknown error"));
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  int optval = 1;
  if (setsockopt(listening_socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0) {
    char *err = ::strerror(errno);
    throw Router::Exception("setsockopt: " + std::string(err? err: "unknown error"));
  }

  if (bind(listening_socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    char *err = ::strerror(errno);
    throw Router::Exception("bind: " + std::string(err? err: "unknown error"));
  }
}
