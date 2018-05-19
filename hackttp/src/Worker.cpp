#include "Worker.h"

char header_buf[HTTP_REQUEST_LENGTH];

void Worker::handle_request(std::string root) {
    ssize_t header_length = recv(socket_fd, header_buf,
                                 HTTP_REQUEST_LENGTH, 0);
    std::string req_str(header_buf, header_length);
    Response resp;

    if (header_length > 0) {
        Request req(req_str);

        if (req.valid) {
            DataHandler dHandler;
            try {
                resp = dHandler.read_resource(req, root);
            } catch (DataHandler::Unsupported &e) {
                resp = Response::fromCode(HTTP_UNSUP_MEDIA_TYPE);
            } catch (DataHandler::Exec::PermissionDenied &e) {
                resp = Response::fromCode(HTTP_FORBIDDEN);
            } catch (DataHandler::Exception &e) {
                resp = Response::fromCode(HTTP_NOT_FOUND);
            }
        } else {
            resp = Response::fromCode(HTTP_BAD_REQUEST);
        }
        std::string resp_str = resp.render();
        send_msg(resp_str.c_str(), resp_str.size());
    }
    exit(0);
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
