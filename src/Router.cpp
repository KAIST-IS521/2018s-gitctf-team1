#include "Router.h"
#include "Worker.h"

#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>


static void handler(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void Router::init_sigchld_handler() {
    signal(SIGCHLD, handler);
}

Router::Router(int q, int p) : queue_size(q), port(p) {
    init_socket();
    init_sigchld_handler();
}

Router::~Router() {
    close(fd);
}

void Router::watch() {
    while (true) {
        struct sockaddr_storage client;
        socklen_t addr_size = sizeof client;
        int client_fd;

        client_fd = accept(fd, (struct sockaddr *) &client, &addr_size);
        if (client_fd < 0) {
            if(errno == EINTR) break;
            else {
                throw Router::Exception(UNIX_ERROR("accept: "));
            }
        }

        pid_t pid = fork();
        if (pid < 0) {
            throw Router::Exception(UNIX_ERROR("fork: "));
        } else if (!pid) { // child
            close(fd); // don't need server fd

            struct sockaddr_in *sin = (struct sockaddr_in *)&client;
            char client_addr[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET, &sin->sin_addr, client_addr, sizeof client_addr);
            std::cout << "New connection: " <<
                client_addr << ":" << sin->sin_port << std::endl;

            Worker w(client_fd); // call worker
            w.handle_request();
        } else {
            close(client_fd); // parent don't need clind fd anymore
        }
    }
}

void Router::init_socket() {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throw Router::Exception(UNIX_ERROR("socket: "));
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    int optval = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0) {
        throw Router::Exception(UNIX_ERROR("setsockopt: "));
    }
    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        throw Router::Exception(UNIX_ERROR("bind: "));
    }
    if (listen(fd, queue_size) < 0) {
        throw Router::Exception(UNIX_ERROR("listen: "));
    }
}
