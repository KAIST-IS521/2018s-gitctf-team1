#include "Response.h"
#include "DataHandler.h"
#include "Util.h"

#include <iostream>

Response *Response::fromCode(int code) {
  DataHandler::Resource s;
  return new Response(code, s);
}

Response::Response() {

}

Response::Response(int code, DataHandler::Resource rsrc) {
    status_code = code;

    switch (status_code) {
        case HTTP_OK:                 status = "OK"; break;
        case HTTP_BAD_REQUEST:        status = "Bad Request"; break;
        case HTTP_FORBIDDEN:          status = "Forbidden"; break;
        case HTTP_NOT_FOUND:          status = "Not Found"; break;
        case HTTP_UNSUP_MEDIA_TYPE:   status = "Unsupported Media Type"; break;
        case HTTP_INTERNAL_SRV_ERROR: status = "Internal Server Error"; break;
        case HTTP_NOT_IMPLEMENTED:    status = "Not Implemented"; break;
        default:                      status = "Unknown error";
    }

    std::string &data = rsrc.data;
    if (data.compare(0, 6, "HTTP/1") == 0) {
        size_t pos = 0, prev_pos = 0;
        std::string tmp;

        if ((pos = data.find(" ", prev_pos)) != std::string::npos) {
            tmp = data.substr(prev_pos, pos - prev_pos);
            prev_pos = pos + 1;
        }
        if (tmp != "HTTP/1.1" && tmp != "HTTP/1.0") {
            // throw HTTP_INTERNAL_SRV_ERROR
        }

        if ((pos = data.find(" ", prev_pos)) != std::string::npos) {
            tmp = data.substr(prev_pos, pos - prev_pos);
            prev_pos = pos + 1;
        }
        if (std::stoi(tmp) != status_code) {
            // throw HTTP_INTERNAL_SRV_ERROR
        }

        // status message -- skip?
        if ((pos = data.find("\r\n", prev_pos)) != std::string::npos) {
            prev_pos = pos + 2;
        }

        while ((pos = data.find("\r\n", prev_pos)) != std::string::npos) {
            std::string line = data.substr(prev_pos, pos - prev_pos);
            if (line == "") break;

            size_t temp_pos = 0;
            if ((temp_pos = line.find(":", 0)) != std::string::npos) {
                std::string key;
                std::string value;

                key = line.substr(0, temp_pos);
                value = line.substr(temp_pos + 1, line.length());
                value = trim(value);

                headers.insert(std::make_pair(strtolower(key), value));
            }

            prev_pos = pos + 2;
        }

        body = data.substr(prev_pos + 2, data.size());

        if (headers.find("content-length") == headers.end())
            headers.insert(std::make_pair("content-length", std::to_string(data.size())));
        else {
            int content_length = std::stoi(headers["content-length"]);
            if (body.size() != content_length) {
                // throw HTTP_INTERNAL_SRV_ERROR
            }
        }

    } else { // maybe no header
        length = data.size();
        headers.insert(std::make_pair("Server", "IS521-2018s-GITCTF-TEAM1"));
        headers.insert(std::make_pair("Content-Type", rsrc.type == ""? "text/plain": rsrc.type));
        headers.insert(std::make_pair("Content-Length", std::to_string(data.size())));
        headers.insert(std::make_pair("Connection", "Close"));
        body = data;
    }
}

std::string Response::render() {
    std::string output;

    output += "HTTP/1.1 " + std::to_string(status_code) + " " + status + "\r\n";
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        output += it->first + ": " + it->second + "\r\n";
    }
    output += "\r\n";
    output += body;

    return output;
}
