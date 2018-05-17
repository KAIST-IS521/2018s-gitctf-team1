#include "../DataHandler.h"

#include <unistd.h>
#include <vector>

#define BUF_BLOCK_SIZE 1024
#define PIPE_READ 0
#define PIPE_WRITE 1

DataHandler::Exec::Exec() { }

DataHandler::Exec::~Exec() { }

DataHandler::resource DataHandler::Exec::run_command(std::string args[]) {
    return DataHandler::Exec::run_command(args, NULL);
}

DataHandler::resource DataHandler::Exec::run_command(std::string args[], DataHandler::resource *data) {
    return DataHandler::Exec::run_command(args, NULL, "");
}

DataHandler::resource DataHandler::Exec::run_command(std::string args[], DataHandler::resource *data, std::string cookies) {
    int comms_in[2];
    int comms_out[2];

    int buf_pos = 0, buf_max, buf_blocks = 1;
    DataHandler::resource output;

    buf_max = BUF_BLOCK_SIZE * buf_blocks;
    output.data = (char *) malloc(buf_max * sizeof(char));

    if (pipe(comms_in) < 0) {
        char *err = std::strerror(errno);
        throw DataHandler::Exception("Cannot create pipe to exec: " + std::string(err ? err : "unknown error"));
    }

    if (pipe(comms_out) < 0) {
        close(comms_in[PIPE_READ]);
        close(comms_in[PIPE_WRITE]);

        char *err = std::strerror(errno);
        throw DataHandler::Exception("Cannot create pipe from exec: " + std::string(err ? err : "unknown error"));
    }

    int pid = fork();
    if (pid == 0) { // child
        int sin = dup(STDIN_FILENO),
            sout = dup(STDOUT_FILENO),
            serr = dup(STDERR_FILENO);

        // redirect STDIN to our comms
        if (dup2(comms_in[PIPE_READ], STDIN_FILENO) == -1) {
            char *err = std::strerror(errno);
            throw DataHandler::Exception("Cannot redirect STDIN to pipe: " + std::string(err ? err : "unknown error"));
        }

        // redirect STDOUT to our comms
        if (dup2(comms_out[PIPE_WRITE], STDOUT_FILENO) == -1) {
            char *err = std::strerror(errno);
            throw DataHandler::Exception("Cannot redirect STDOUT to pipe: " + std::string(err ? err : "unknown error"));
        }

        // same with STDERR
        if (dup2(comms_out[PIPE_WRITE], STDERR_FILENO) == -1) {
            char *err = std::strerror(errno);
            throw DataHandler::Exception("Cannot redirect STDERR to pipe: " + std::string(err ? err : "unknown error"));
        }

        // now close the pipes on our side
        close(comms_in[PIPE_WRITE]);
        close(comms_out[PIPE_READ]);

        std::vector<std::string> envs;
        // set the request method correctly
        if (data) {
          envs.push_back("REQUEST_METHOD=" + data->type);
          envs.push_back("CONTENT_LENGTH=" + std::to_string(data->size + 1));

            // don't forget to pass any GET data we might have got for the executed command
            if (data->type == "GET") {
              envs.push_back("QUERY_STRING=" + std::string(data->data));
            }
        }

        if (cookies.length() > 0) {
          envs.push_back("COOKIE=" + cookies);
        }

        for (auto it = envs.begin(); it != envs.end(); ++it)
          if (putenv((char *)(*it).c_str()) != 0)
            throw DataHandler::Exception("Error on putenv");

        // now run the target
        // TODO: Fix me! I'm ugly!
        if (execl(args[0].c_str(), args[0].c_str(), args[1].c_str(), (char *) 0) < 0) {
            char * err = std::strerror(errno);
            // close everything before leaving
            close(comms_in[PIPE_READ]);
            close(comms_out[PIPE_WRITE]);
            // and restore IN/OUT streams
            dup2(sin,  STDIN_FILENO);  close(sin);
            dup2(sout, STDOUT_FILENO); close(sout);
            dup2(serr, STDERR_FILENO); close(serr);
            // finally throw the exception
            throw DataHandler::Exec::PermissionDenied(
                "Cannot exec '" + std::string(args[0]) + "' due to: " + std::string(err ? err : "unknown error")
            );
        }
    }
    else if (pid > 0) { // parent
        close(comms_in[PIPE_READ]);
        close(comms_out[PIPE_WRITE]);

        // Now first write what we have
        if (data && data->type == "POST") {
            if (write(comms_in[PIPE_WRITE], data->data, data->size + 1) == -1 ) {
                char *err = std::strerror(errno);
                throw DataHandler::Exception(
                    "Error while writing to child process: " + std::string(args[0]) +", "
                    + "error message: " + std::string(err ? err : "unknown error")
                );

            }
        }
        close(comms_in[PIPE_WRITE]);

        char rchar;
        while (read(comms_out[PIPE_READ], &rchar, 1) == 1) {
            output.data[buf_pos++] = rchar;
            if (buf_pos >= buf_max){
                buf_blocks++;
                buf_max = BUF_BLOCK_SIZE * buf_blocks;

                output.data = (char *) realloc(output.data, buf_max * sizeof(char));
                if (output.data == NULL) {
                    char *err = std::strerror(errno);
                    throw DataHandler::Exception(
                        "Error while reading output from: " + std::string(args[0]) +", "
                        + "because of: " + std::string(err ? err : "unknown error")
                    );
                }
            }
        }

        // add NULL termination
        output.data[buf_pos] = '\0';
        output.size = buf_pos;

        close(comms_out[PIPE_READ]);
    }
    else { // fork failed
        char * err = std::strerror(errno);
        close(comms_in[PIPE_READ]);
        close(comms_in[PIPE_WRITE]);
        close(comms_out[PIPE_READ]);
        close(comms_out[PIPE_WRITE]);
        throw DataHandler::Exception("Cannot fork: " + std::string(err ? err : "unknown error"));
    }

    if (strstr(output.data, "DataHandler::Exception") != NULL) {
        throw DataHandler::Exception("Fork returned an exception: " + std::string(output.data));
    }

    return output;
}
