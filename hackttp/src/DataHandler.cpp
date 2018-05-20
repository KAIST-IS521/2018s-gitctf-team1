#include "DataHandler.h"
#include "Exec.h"
#include <linux/limits.h>
#include <fcntl.h>
#include <unistd.h>
#include "Response.h"

using namespace std;

DataHandler::DataHandler() { }

DataHandler::~DataHandler() {}

bool DataHandler::verify_path(std::string path) {
    bool path_ok = false;

    // verify that the path is not all slashes
    // there's a very nice uncaught exception when we pass '//+' to open()
    for(char c : path) {
        if (c != '/') {
            path_ok = true;
            break;
        }
    }

    return path_ok;
}

Response *DataHandler::read_Resource(Request req, std::string path) {

	if(!verify_path(path)){
		path = "/index.html";
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

    // std::string mime = run_command("", { "/usr/bin/file", pname });

    DataHandler::Resource output;
    std::string script_output = run_command(path, req);
    output.data = script_output;
    output.type = "executable";

    return new Response(HTTP_OK, output);
}
