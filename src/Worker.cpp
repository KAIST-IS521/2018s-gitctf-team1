#include "Response.h"
#include "Request.h"
#include "DataHandler.h"
#include "Worker.h"

#include <sys/socket.h>
#include <unistd.h>

#define HTTP_REQUEST_LENGTH 8192

char header_buf[HTTP_REQUEST_LENGTH];

void Worker::handle_request() {
    ssize_t header_length = recv(socket_fd, header_buf,
                                 HTTP_REQUEST_LENGTH, 0);
    std::string req_str(header_buf, header_length);
    int code = HTTP_OK;
    std::string data("");

    if (header_length > 0) {
        request req(req_str);

        if (req.valid) {
            DataHandler dHandler;
            try {
                data = dHandler.read_resource(req);
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

        response resp(code, data);
        resp.render(socket_fd);
    }
}

void Worker::send_msg(char * msgc, long size) {
    ssize_t cur = 0;
    ssize_t nbytes = 0;
    while (cur < size) {
        if ((nbytes = send(socket_fd, msgc + cur, size - cur, 0)) < 0) {
            throw Worker::Exception(UNIX_ERROR("send"));
        }
        cur += nbytes;
    }
}
