#include "Exec.h"

std::string run_command(std::string path, Request req) {
  std::vector<std::string> args;
  args.push_back(req.uri);

  std::vector<std::string> envs;
  envs.push_back("REQUEST_METHOD=" + req.method);
  envs.push_back("CONTENT_LENGTH=" + std::to_string(req.getPostData().size()));
  auto qstr = req.getQueryString();
  if (qstr.length() > 0) {
      envs.push_back("QUERY_STRING=" + qstr);
  }

  auto cookie = req.getCookie();
  if (cookie.length() > 0) {
      envs.push_back("COOKIE=" + cookie);
  }

  return run_command(path, args, envs, req.getPostData());
}

std::string run_command(std::string path, std::vector<std::string> args) {
  std::vector<std::string> envs;
  return run_command(path, args, envs, "");
}

std::string run_command(
  std::string path,
  std::vector<std::string> args,
  std::vector<std::string> envs,
  std::string data
) {
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

        for (auto it = envs.begin(); it != envs.end(); ++it) {
          if (putenv((char *)(*it).c_str()) != 0)
            throw DataHandler::Exception("Error on putenv");
        }

        std::string tmp = args[0];
        ReplaceAll(tmp, "..", "");
        tmp = path + tmp;
        auto pname = tmp.c_str();
        int ret;
        if (args.size() == 2) {
          auto arg = args[1].c_str();
          ret = execl(pname, pname, arg, NULL);
        } else {
          ret = execl(pname, pname, NULL);
        }
        if (ret < 0) {
            exit(-1);
        }
    }
    else if (pid > 0) { // parent
        close(comms_in[PIPE_READ]);
        close(comms_out[PIPE_WRITE]);

        // Now first write what we have
        if (data.length() > 0) {
            write(comms_in[PIPE_WRITE], data.c_str(), data.length());
        }
        close(comms_in[PIPE_WRITE]);

        int status;
        char rchar;
        while (read(comms_out[PIPE_READ], &rchar, 1) == 1) {
            out += rchar;
        }
        close(comms_out[PIPE_READ]);

        if (waitpid(pid, &status, WNOHANG) < 0) {
            throw DataHandler::Exception(UNIX_ERROR("waitpid"));
        }
        if (WIFEXITED(status) && !WEXITSTATUS(status)) { }
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
