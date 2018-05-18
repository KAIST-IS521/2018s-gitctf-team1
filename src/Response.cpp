#include "Response.h"

BasicHTTP::response BasicHTTP::render_headers(int code, DataHandler::resource rsrc) {
    response resp;

    // by default no headers are created
    resp.has_headers = false;
    if (rsrc.type != "executable") {
        resp.headers =
            "HTTP/1.1 "+std::to_string(code)+" OK\n"
            "Server: HackTTP\n"
            "Connection: close\n"
            "Content-Type: "+rsrc.type+"\n";
        ;

        if (rsrc.type.find("image/") != std::string::npos) {
            resp.headers += "Accept-Ranges: bytes\n";
            resp.headers += "Content-Length: " + std::to_string(rsrc.size) + "\n";
        }

        // now send the headers
        resp.headers += "\n";
        resp.has_headers = true;
    }
    return resp;
}


