#pragma once

#include "Exceptions.h"
#include "Request.h"
#include "DataHandler.h"
#include "Util.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#define PIPE_READ 0
#define PIPE_WRITE 1

// expected path : /var/www/html (like this without '/' terminating)
std::string run_command(std::string path, Request req);
std::string run_command(std::string path, std::vector<std::string> args);
std::string run_command(
  std::string path,
  std::vector<std::string> args,
  std::vector<std::string> envs,
  std::string data
);
