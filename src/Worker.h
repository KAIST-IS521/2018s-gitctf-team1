#ifndef SRC_WORKER_H_
#define SRC_WORKER_H_


class Worker {
private:
    int socket_fd;
    void send_msg(char * msg, long size);
public:
    Worker(int fd) : socket_fd(fd) { };
    virtual ~Worker();
    void handle_request();

    class Exception: public BaseException {
        public:
            Exception(std::string msg = "Unknown worker exception") {
                this->reason = msg;
            }
    };
};

#endif /* SRC_WORKER_H_ */
