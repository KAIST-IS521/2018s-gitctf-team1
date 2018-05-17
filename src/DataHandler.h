#ifndef SRC_DATAHANDLER_H_
#define SRC_DATAHANDLER_H_

#include "Exceptions.h"
#include <cstring>
#include <cerrno>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace std;

class DataHandler {
public:
    struct resource {
        string type;
        char * data;
        long size;
    };

private:
    std::string get_working_path();
    bool verify_path(std::string path);
public:
    DataHandler(string client);
    virtual ~DataHandler();
    resource read_resource(std::string path);
    resource read_resource(std::string path, std::string cookies);
    resource read_resource(std::string path, std::string cookies, DataHandler::resource * data);
    resource get_error_file(int error_code, std::string param);

    // Exception base
    class Exception: public BaseException {
        public:
            Exception(std::string msg = "Unknown data handler exception") {
                this->reason = msg;
            }
    };

    class Exec {
        public:
            Exec();
            ~Exec();
            resource run_command(std::string args[]);
            resource run_command(std::string args[], DataHandler::resource * data);
            resource run_command(std::string args[], DataHandler::resource * data, std::string cookies);

        class PermissionDenied: public Exception {
            public:
                PermissionDenied(std::string msg = "Permission denied while trying to execute command") {
                    this->reason = msg;
                }
        };
    };

    class FileNotFound: public Exception {
        public:
            FileNotFound(std::string msg = "Could not find requested file") {
                this->reason = msg;
            }
    };
    class Unsupported: public Exception {
        public:
            Unsupported(std::string msg = "Unsupported file found") {
                this->reason = msg;
            }
    };
};

#endif /* SRC_DATAHANDLER_H_ */
