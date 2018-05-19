#include "DataHandler.h"
#include "Exec.h"
#include <linux/limits.h>
#include <fcntl.h>
#include <unistd.h>

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

bool is(char *mime, std::string type) {
    return (strstr(mime, type.c_str()) != NULL);
}

DataHandler::Resource DataHandler::read_Resource(Request req, std::string path) {
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
    //Exec runner;

    std::string tmp = req.uri;
    ReplaceAll(tmp, "..", "");
    tmp = path + tmp;
    auto pname = tmp.c_str();

    std::string mime = run_command("", { "/usr/bin/file", pname });
    //char *mime = file_mime.data;
    //std::string ext = path.substr(path.length() - 3);
    //for(char c : ext)
    //    c = std::toupper(c); 

    DataHandler::Resource output;
	//if(is(mime, "executable")){
        std::string script_output = run_command(path, req);
        output.data = script_output;
        output.type = "executable";
    // }

    if (output.type.length() == 0){
        std::string error_str = "Unsupported mime type: " + std::string(mime);
        // drop 'local' part of path
        size_t pos = 0;
        while ((pos = error_str.find(cwd)) != std::string::npos)
            error_str.erase(pos, cwd.length());
        throw DataHandler::Unsupported(error_str);
    }

	//Response resp;
	//resp = Response::Response(HTTP_OK, output);
    return output;
}

//#define PATH_MAX 1000
std::string DataHandler::get_working_path() {
   //int PATH_MAX = 1000;
   char temp[PATH_MAX]; 
   return ( getcwd(temp, PATH_MAX) ? std::string(temp) : std::string("") );
} 
/*
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
} */
