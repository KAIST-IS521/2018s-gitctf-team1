#include "Request.h"
#include "Util.h"
#include <string>
#include <curl/curl.h>
#include <string.h>
#include <iostream>

Request::Request(std::string req_str) {
    fetch_headers(req_str);
    if (valid) {
        fetch_cookies();
        fetch_queries();
        if (method == "POST") {
            fetch_forms();
        }
    }
}

std::string Request::getHeader(std::string key){
    return header.find(strtolower(key)) == header.end() ? "": header[key];
}

std::string Request::getParameters(std::map<std::string, std::string> target, char delimiter='&'){
    std::string result;

    for (auto it = target.begin(); it != target.end(); ++it) {
        if (it != target.begin()) {
            result += delimiter;
        }
        result += urlencode(it->first) + "=" + urlencode(it->second);
    }

    return result;
}

std::string Request::getQueryString() {
    return getParameters(get);
}

std::string Request::getPostData() {
    if (getHeader("content-type") == "application/x-www-form-urlencoded")
        return getParameters(post);
    std::string temp = getHeader("content-length");
    if (temp != "")
        return raw_data.substr(0, std::stoi(temp));
    return raw_data;
}

std::string Request::getCookie() {
    return getParameters(cookie, ';');
}

Request::~Request() { }

void Request::fetch_headers(std::string req_str) {
    if (req_str.length() < 3) {
        valid = false;
        return;
    }

    size_t pos = 0, prev_pos = 0;

    // method
    if ((pos = req_str.find(" ", prev_pos)) != std::string::npos) {
        method = req_str.substr(prev_pos, pos - prev_pos);
        prev_pos = pos + 1;
    }

    // uri
    if ((pos = req_str.find(" ", prev_pos)) != std::string::npos) {
        uri = req_str.substr(prev_pos, pos - prev_pos);
        prev_pos = pos + 1;
    }

    // version
    if ((pos = req_str.find("\r\n", prev_pos)) != std::string::npos) {
        http_version = req_str.substr(prev_pos, pos - prev_pos);
        prev_pos = pos + 2;
    }

    // check validity of request
    valid = ((http_version == "HTTP/1.0" || http_version == "HTTP/1.1") &&
            (method == "GET" || method == "POST") &&
            (uri.length() > 0));

    while ((pos = req_str.find("\r\n", prev_pos)) != std::string::npos) {
        std::string line = req_str.substr(prev_pos, pos - prev_pos);
        if (line == "") break;

        size_t temp_pos = 0;
        if ((temp_pos = line.find(":", 0)) != std::string::npos) {
            std::string key;
            std::string value;

            key = line.substr(0, temp_pos);
            value = line.substr(temp_pos + 1, line.length());
            value = trim(value);

            header.insert(std::make_pair(strtolower(key), value));
        }

        prev_pos = pos + 2;
    }
    raw_data = req_str.substr(prev_pos + 2, req_str.size());
}


void Request::fetch_cookies() {
    size_t kpos = 0, vpos = 0;

    std::string l_cookies = getHeader("cookie");
    // to extract the POST params we first need to find it and it's length
    if (l_cookies != "") {
        while ((kpos = l_cookies.find("=", vpos)) != std::string::npos) {
            std::string key;
            std::string value;
            key = l_cookies.substr(vpos, kpos - vpos);

            if ((vpos = l_cookies.find(";", kpos)) != std::string::npos) {
                value = l_cookies.substr(kpos + 1, vpos - kpos - 1);
                vpos = vpos + 2;
            } else {
                vpos = l_cookies.find("\r\n", kpos);
                value = l_cookies.substr(kpos + 1, vpos - kpos);
            }
            key = urldecode(key);
            value = urldecode(value);
            cookie.insert(std::make_pair(strtolower(key), value));
        }
    }
}

//get
void Request::fetch_queries() {
    size_t pos = 0, kpos = 0, vpos = 0, tpos = 0;
    std::string params = "";

    if ((pos = uri.find("?", 0)) != std::string::npos) {
        params = uri.substr(pos + 1);
        uri = uri.substr(0, pos);

        while ((kpos = params.find("=", vpos)) != std::string::npos) {
            std::string key;
            std::string value;
            key = params.substr(vpos, kpos - vpos);
	    if ((tpos = key.find("&", 0)) != std::string::npos) {
		key = key.substr(0,tpos);
		key = urldecode(key);
		get.insert(std::make_pair(strtolower(key),""));
	        vpos = tpos + vpos + 1;
		continue;
	    }
            if ((vpos = params.find("&", kpos)) != std::string::npos) {
                value = params.substr(kpos + 1, vpos - kpos - 1);
                vpos = vpos + 1;
            } else {
                vpos = params.find("\r\n", kpos);
                value = params.substr(kpos + 1, vpos - kpos);
            }
            key = urldecode(key);
            value = urldecode(value);
            get.insert(std::make_pair(strtolower(key), value));
        }
    }
}

//post
void Request::fetch_forms() {
    size_t kpos = 0, vpos = 0;
    std::string data = "";

    // to extract the POST params we first need to find it and it's length
    std::string content_length_str = getHeader("content-length");
    if (content_length_str != "") {
        int content_length = std::stoi(content_length_str);

        data = raw_data.substr(0, content_length);
        while ((kpos = data.find("=", vpos)) != std::string::npos) {
            std::string key;
            std::string value;
            key = data.substr(vpos, kpos - vpos);
            if ((vpos = data.find("&", kpos)) != std::string::npos) {
                value = data.substr(kpos + 1, vpos - kpos - 1);
                vpos = vpos + 1;
            } else {
                vpos = data.find("\r\n", kpos);
                value = data.substr(kpos + 1, vpos - kpos);
            }
            key = urldecode(key);
            value = urldecode(value);
            post.insert(std::make_pair(strtolower(key), value));
        }
    }
}
