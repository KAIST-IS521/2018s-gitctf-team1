#include "BasicHTTP.h"
#include "DataHandler.h"
#include "Worker.h"

#include <sys/socket.h>
#include <unistd.h>

#define HTTP_REQUEST_LENGTH 8192

char header_buf[HTTP_REQUEST_LENGTH];

void Worker::handle_request() {
    ssize_t header_length = recv(this->socket_fd, header_buf,
                                 HTTP_REQUEST_LENGTH, 0);
    DataHandler::resource data;
    int code = HTTP_OK;

    if (header_length <= 0) {
        return;
    } else {
        BasicHTTP *httpHandler = (BasicHTTP *)   new BasicHTTP("A");
        DataHandler *dHandler  = (DataHandler *) new DataHandler("A");

        std::string req_str(header_buf, header_length);
        BasicHTTP::request req = httpHandler->parse_request(req_str);

        if (req.valid) {
            try {
                data = dHandler->read_resource(req.uri, req.cookies, &req.data);
            } catch (DataHandler::Unsupported &e) {
                code = HTTP_UNSUP_MEDIA_TYPE;
            } catch (DataHandler::Exec::PermissionDenied &e) {
                code = HTTP_FORBIDDEN;
            } catch (DataHandler::Exception &e) {
                code = HTTP_NOT_FOUND;
            }
        } else {
            code = HTTP_BAD_REQUEST;
        }

        BasicHTTP::response resp = httpHandler->render_headers(code, data);
        if (resp.has_headers) {
            send_msg((char *) resp.headers.c_str(), resp.headers.length());
        }
        send_msg(data.data, data.size);
        delete httpHandler;
        delete dHandler;
        // TODO: Also need to check dynamic allocation
        //free(req); // XXX: check
        //free(data.data); // XXX: check
        close(this->socket_fd);
        return;
    }
}

void Worker::send_msg(char * msgc, long size) {
    ssize_t cur = 0;
    ssize_t nbytes = 0;
    while (cur < size) {
        if ((nbytes = send(this->socket_fd, msgc + cur, size - cur, 0)) < 0) {
            char * err = std::strerror(errno);
            throw Worker::Exception("send_msg::send fail: " + std::string(err ?
                        err : "unknown error"));
        }
        cur += nbytes;
    }
}
