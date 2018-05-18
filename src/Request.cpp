#include "Request.h"
#include <algorithm>

request::request(string req_str) {
    std::transform(req_str.begin(), req_str.end(),
                   req_str.begin(), std::tolower);
    fetch_headers(req_str);
    if (valid) {
        fetch_cookies(req_str);
        fetch_queries(req_str);
        fetch_forms(req_str);
    }
}

/* From http://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html#sec5
 * Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
 */
void request::fetch_headers(string req_str) {
    if (req_str.length() < 3) {
        req.valid = false;
        return;
    }


    size_t pos = 0, prev_pos = 0;

    // XXX: method
    if ((pos = req_str.find(" ", prev_pos)) != std::string::npos) {
        method = req_str.substr(prev_pos, pos-prev_pos);
        prev_pos = pos + 1;
    }

    // XXX: uri
    if ((pos = req_str.find(" ", prev_pos)) != std::string::npos) {
        uri = req_str.substr(prev_pos, pos-prev_pos);
        prev_pos = pos + 1;
    }

    // XXX: version
    std::string http_ver;
    if ((pos = req_str.find("\r\n", prev_pos)) != std::string::npos) {
        http_version = req_str.substr(prev_pos, pos-prev_pos);
        prev_pos = pos + 2;
    }

    // XXX: check validity of request
    valid = ((http_version == "http/1.0" || req.http_version == "http/1.1") &&
            (method == "get" || method == "post") &&
            (uri.length() > 0));
}

/*
std::string BasicHTTP::fetch_cookies(std::string req_str) {
    size_t pos = 0, eol = 0;

    std::string cookies = "";
    // to extract the POST params we first need to find it and it's length
    if ((pos = req_str.find("cookie: ")) != std::string::npos) {
        if ((eol = req_str.find("\r\n", pos)) != std::string::npos) {
            cookies = req_str.substr(pos + 8, eol - pos - 8);
        }
    }

    return cookies;
}

void BasicHTTP::handle_get_method(BasicHTTP::request * req) {
    size_t pos = 0;

    req->data.type = req->method;
    if ((pos = req->uri.find("?", 0)) != std::string::npos) {
        req->data.size = req->uri.length() - pos + 1;
        req->data.data = (char *) malloc((req->data.size+1) * sizeof(char));
        std::string params = req->uri.substr(pos+1);
        for (int i = 0; i < req->data.size; i++)
            req->data.data[i] = params[i];
        req->uri  = req->uri.substr(0, pos);
    }
    else {
        req->data.size = 0;
        req->data.data = (char *) malloc((req->data.size+1) * sizeof(char));
        req->data.data[0] = '\0';
    }
}

void BasicHTTP::handle_post_method(BasicHTTP::request * req, std::string req_str) {
    size_t pos = 0, prev_pos = 0;

    req->data.type = req->method;
    if ((pos = req->uri.find("?", 0)) != std::string::npos)
        req->uri  = req->uri.substr(0, pos);

    // to extract the POST params we first need to find it and it's length
    if ((pos = req_str.find("content-length:", prev_pos)) != std::string::npos) {
        size_t eol = 0;
        int content_length = 0;
        if ((eol = req_str.find("\r\n", pos)) != std::string::npos) {
            content_length = std::stoi(req_str.substr(pos + 16, eol - pos + 16));
        }
        // don't read anything, if content data was 0, or wasn't defined at all
        if (content_length > 0 && (pos = req_str.find("\r\n\r\n", eol)) != std::string::npos) {
            req->data.type = req->method;
            req->data.size = content_length;
            req->data.data = (char *) malloc((req->data.size + 1)* sizeof(char));
            std::string data = req_str.substr(pos+4);
            for (int i = 0; i < req->data.size; i++)
                req->data.data[i] = data[i];
            req->data.data[req->data.size] = '\0';
        }
    }
}
*/
