#include "DataHandler.h"
#include "Exec.h"
#include <linux/limits.h>
#include <fcntl.h>
#include <unistd.h>
#include "Response.h"

using namespace std;

DataHandler::DataHandler() { }

DataHandler::~DataHandler() {}

Response *DataHandler::read_Resource(Request req, std::string path) {
	if (req.uri == "/"){
		req.uri = "/index.py";
	}

  std::string tmp = req.uri;
  ReplaceAll(tmp, "..", "");
  tmp = path + tmp;
  auto pname = tmp.c_str();

  // first check if such a file even exists
  int fd = open(pname, O_RDONLY);
  if (fd < 0) {
    char *err = std::strerror(errno);
    throw DataHandler::FileNotFound("Error while reading file contents at " + path + ": " + std::string(err ? err : "unknown error"));
  }
  close(fd);

  DataHandler::Resource output;
  std::string script_output = run_command(path, req);
  output.data = script_output;
  output.type = "executable";

  return new Response(HTTP_OK, output);
}
