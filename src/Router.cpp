#include "Router.h"
#include "Manager.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

/*
 * router - watches the designated port and for launches a new worker for every connection
 * (when all workers are busy, waits some time before returning an error - "how long" should be kept in the config)
 * Handles *some* of the HTTP errors
 */

Router::Router(int qsize, int port) {
    this->queue_size = qsize;
    this->port = port;
    // initialize the socket
    this->init_socket();
}

Router::~Router() {
    // close our socket
    close(this->listening_socket_fd);
    // free the address
    //freeaddrinfo(addr);
    //this->logger->info("Router instance correctly closed, dropping Logger");
}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void Router::watch() {
    socklen_t addr_size;
    struct sockaddr_storage client;
    int handling_socket;

    //this->logger->debug("watching port: " + this->port);

    // create a manager object to handle different threads
    Manager manager;

    // we work until we're told to stop working
    while (!isSigintReceived) {
        // listen for new connections
        listen(this->listening_socket_fd, this->queue_size);

        addr_size = sizeof client;
        handling_socket = accept(this->listening_socket_fd, (struct sockaddr *) &client, &addr_size);
        if (handling_socket < 0) {
            if(errno == EINTR){
                //this->logger->info("Accept() interrupted by signal");
                break;
            } else {
                char *err = std::strerror(errno);
                close(this->listening_socket_fd);
                throw Router::Exception("Error when accepting connection: " + std::string(err ? err : "unknown error"));
            }
        }

        struct sockaddr_in *sin = (struct sockaddr_in *)&client;
        char client_addr[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET, &sin->sin_addr, client_addr, sizeof client_addr);
        //this->logger->debug("Handling connection from: " + string(client_addr));
        // this will create a new worker to work with
        // TODO: add try/catch and handle too many workers exception
        manager.handle_request(string(client_addr), handling_socket);
    }
}

void Router::init_socket() {
    // XXX: only support ipv4
    listening_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (listening_socket_fd < 0) {
        char * err = std::strerror(errno);
        throw Router::Exception("socket: " + std::string(err ?
                    err : "unknown error"));
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_UNSPEC;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    int optval = 1;
    if (setsockopt(listening_socket_fd, SOL_SOCKET, SO_REUSEADDR,
                   &optval, sizeof(int)) < 0) {
        char *err = std::strerror(errno);
        throw Router::Exception("setsockopt: " + std::string(err ?
                    err : "unknown error"));
    }

    if (bind(listening_socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        char *err = std::strerror(errno);
        throw Router::Exception("bind: " + std::string(err ?
                    err : "unknown error"));
    }
}
