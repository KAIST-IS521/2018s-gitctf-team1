#pragma once

#include "Exceptions.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/limits.h>
#include <arpa/inet.h>
#include <string>

class Router {
    private:
        int queue_size;
        int fd;
        int port;
        struct sockaddr_in addr;
        void init_socket();
        void init_sigchld_handler();
        std::string root;

    public:
        Router(int qsize, int port, std::string r);
        virtual ~Router();
        void watch();

        class Exception: public BaseException {
            public:
                Exception(std::string msg = "Unknown router exception") {
                    reason = msg;
                }
        };
};
