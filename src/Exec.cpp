#include "../DataHandler.h"
#include "../Request.h"

#include <unistd.h>
#include <vector>

#define PIPE_READ 0
#define PIPE_WRITE 1

std::string run_command(Request req) {
    int comms_in[2];
    int comms_out[2];

    std::string out;

    if (pipe(comms_in) < 0) {
        throw DataHandler::Exception(UNIX_ERROR("pipe"));
    }

    if (pipe(comms_out) < 0) {
        close(comms_in[PIPE_READ]);
        close(comms_in[PIPE_WRITE]);
        throw DataHandler::Exception(UNIX_ERROR("pipe"));
    }

    int pid = fork();
    if (!pid) { // child
        // redirect STDIN to our comms
        if (dup2(comms_in[PIPE_READ], STDIN_FILENO) == -1 ||
            dup2(comms_out[PIPE_WRITE], STDOUT_FILENO) == -1 ||
            dup2(comms_out[PIPE_WRITE], STDERR_FILENO) == -1) {
            throw DataHandler::Exception(UNIX_ERROR("dup2"));
        }

        // now close the pipes on our side
        close(comms_in[PIPE_WRITE]);
        close(comms_out[PIPE_READ]);

        std::vector<std::string> envs;
        // set the request method correctly
        envs.push_back("REQUEST_METHOD=" + req.method);
        envs.push_back("CONTENT_LENGTH=" + std::to_string(req.raw_data.size()));
        auto qstr = req.getQueryString();
        if (qstr.length() > 0) {
            envs.push_back("QUERY_STRING=" + qstr);
        }
        auto cookie = req.getCookie();
        if (cookie.length() > 0) {
            envs.push_back("COOKIE=" + cookies);
        }

        for (auto it = envs.begin(); it != envs.end(); ++it) {
          if (putenv((char *)(*it).c_str()) != 0)
            throw DataHandler::Exception("Error on putenv");
        }

        auto pname = req.uri.c_str();

        // now run the target
        if (execl(pname, pname, NULL) < 0) {
            exit(-1);
        }
    }
    else if (pid > 0) { // parent
        close(comms_in[PIPE_READ]);
        close(comms_out[PIPE_WRITE]);

        // Now first write what we have
        auto data = req.getPostData();
        if (data.length() > 0) {
            write(comms_in[PIPE_WRITE], data.c_str(), data.length());
        }
        close(comms_in[PIPE_WRITE]);

        int status;
        if (waitpid(pid, &status, WNOHANG) < 0) {
            throw DataHandler::Exception(UNIX_ERROR("waitpid"));
        }

        if (WIFEEXITED(status) && !WEXITSTATUS(status)) {
            char rchar;
            while (read(comms_out[PIPE_READ], &rchar, 1) == 1) {
                out += std::string(rchar);
            }
        }
        close(comms_out[PIPE_READ]);
    }
    else { // fork failed
        close(comms_in[PIPE_READ]);
        close(comms_in[PIPE_WRITE]);
        close(comms_out[PIPE_READ]);
        close(comms_out[PIPE_WRITE]);
        throw DataHandler::Exception(UNIX_ERROR("fork"));
    }

    return out;
}
