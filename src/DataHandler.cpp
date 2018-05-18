#include "DataHandler.h"

#include <linux/limits.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

DataHandler::DataHandler(std::string client) {}

DataHandler::~DataHandler() {}

bool DataHandler::verify_path(std::string path) {
    bool path_ok = false;

    // verify that the path is not all slashes
    // there's a very nice uncaught exception when we pass '//+' to open()
    for(char c : path) {      // is it really works?
        if (c != '/') {  
            path_ok = true;
            break;
        }
    }

    return path_ok;
}

bool is(char *mime, std::string type) {
    return (strstr(mime, type.c_str()) != NULL);
}

DataHandler::resource DataHandler::read_resource(std::string path) {
    return DataHandler::read_resource(path, "");
}
DataHandler::resource DataHandler::read_resource(std::string path, std::string cookies) {
    return DataHandler::read_resource(path, cookies, NULL);
}
DataHandler::resource DataHandler::read_resource(std::string path, std::string cookies, DataHandler::resource *data) {
    // prepend cwd() to path

    if (!verify_path(path)){
        path = "/index.html";
	}

    std::string cwd = get_working_path();
    path = cwd + path;

    // first check if such a file even exists
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        char *err = std::strerror(errno);
        throw DataHandler::FileNotFound("Error while reading file contents at " + path + ": " + std::string(err ? err : "unknown error"));
    }
    close(fd);

    // check mime type of resource
    DataHandler::Exec runner;
    std::string args[2] = { "/usr/bin/file", path };
    DataHandler::resource file_mime = runner.run_command(args);
    char *mime = file_mime.data;
    /* std::string ext = path.substr(path.length()-3);
    for(char c : ext)
        c = std::toupper(c); */ 

    DataHandler::resource output;
    // now check for known mime types
    if (is(mime, "executable")) {
        // run the script, pass the data
        std::string args[2] = { path, "" }; // TODO: Fix me too!
        DataHandler::resource script_output = runner.run_command(args, data, cookies);
        output.data = script_output.data;
        output.size = script_output.size;
        output.type = "executable";
    }
    if (output.type.length() == 0){
        std::string error_str = "Unsupported mime type: " + std::string(mime);
        // drop 'local' part of path
        size_t pos = 0;
        while ((pos = error_str.find(cwd)) != std::string::npos)
            error_str.erase(pos, cwd.length());
		// TODO: fix it!
        throw DataHandler::Unsupported(error_str);
    }

    return output;
}

std::string DataHandler::get_working_path() {
   char temp[PATH_MAX]; // TODO: define PATH_MAX
   return ( getcwd(temp, PATH_MAX) ? std::string(temp) : std::string("") );
}

DataHandler::resource DataHandler::get_error_file(int error_code, std::string param) {
    // start by reading the error template
    DataHandler::resource output = DataHandler::read_resource("/errors/"+ std::to_string(error_code) +".html");
    // now prepare a place to write the filled template
    long new_size = output.size + param.length() - 3;
    char *data = (char *) malloc((new_size+1) * sizeof(char));
    // and fill it
    sprintf(data, output.data, param.c_str());

    // free old structure
    free(output.data);
    // replace with new pointer
    output.data = data;
    output.size = new_size;

    return output;
}
