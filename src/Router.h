#ifndef SRC_ROUTER_H_
#define SRC_ROUTER_H_

#include "Exceptions.h"
#include "Logger.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


class Router {
private:
    int queue_size;
    int listening_socket_fd;
    int port;
    struct sockaddr_in addr;
    Logger *logger;
    void init_socket();

public:
    Router(int qsize, int port);
    virtual ~Router();
    void watch();

    class Exception: public BaseException {
        public:
            Exception(std::string msg = "Unknown router exception") {
                reason = msg;
            }
    };
};

#endif /* SRC_ROUTER_H_ */
