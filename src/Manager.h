#ifndef SRC_MANAGER_H_
#define SRC_MANAGER_H_

#include "Exceptions.h"
#include <pthread.h>
#include <string>
#include <iostream>
#include <cerrno>
#include <cstdlib>
#include <cstring>

using namespace std;

class Manager {
private:
    int worker_count;
    pthread_t * pool; // keeps a pool of available workers
    int get_free_worker_index();

public:
    Manager();
    virtual ~Manager();
    void handle_request(string client, int socket_fd);

    class Exception: public BaseException {
        public:
            Exception(string msg = "Unknown manager exception") {
                reason = msg;
            }
    };
};

#endif /* SRC_MANAGER_H_ */
