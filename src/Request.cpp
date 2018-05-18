#include "Request.h"
#include <algorithm>
#include <curl/curl.h>

std::string tolower(std::string &str) {
  std::transform(str.begin(), str.end(), str.begin(), std::tolower);
  return str;
}

Request::Request(std::string req_str) {
	fetch_headers(req_str);
	if (valid) {
		fetch_cookies(req_str);
		fetch_queries(req_str);
    if (method == "POST") {
		  fetch_forms(req_str);
    }
	}
}

Request::~Request() { }

void Request::fetch_headers(std::string req_str) {
	if (req_str.length() < 3) {
		req.valid = false;
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
	valid = (
    (http_version == "HTTP/1.0" || http_version == "HTTP/1.1") &&
		(method == "GET" || method == "POST") &&
		(uri.length() > 0)
  );

  while ((pos = req_str.find("\r\n", prev_pos)) != std::string::npos) {
    std::string line = req_str.substr(prev_pos, pos - prev_pos);
    if (line == "") break;

    size_t temp_pos = 0,
    if ((temp_pos = line.find(":", 0)) != std::string::npos) {

    }

    prev_pos = pos + 2;
  }
}


void Request::fetch_cookies(std::string req_str) {
	size_t pos = 0, eol = 0, kpos=0, vpos =0;

	std::string l_cookies = "";
	// to extract the POST params we first need to find it and it's length
	if ((pos = req_str.find("cookie: ")) != std::string::npos) {
		if ((eol = req_str.find("\r\n", pos)) != std::string::npos) {
			l_cookies = req_str.substr(pos + 8, eol - pos - 8);

			while((kpos = l_cookies.find("=",vpos))!=std::string::npos){
				std::string key="";
				std::string value="";
				key = l_cookies.substr(vpos,kpos-vpos);

				if((vpos = l_cookies.find(";",kpos))!=std::string::npos){
					value = l_cookies.substr(kpos+1,vpos-kpos-1);
					vpos = vpos+2;
				}
				else{
					vpos = l_cookies.find("\r\n",kpos);
					value = l_cookies.substr(kpos+1,vpos-kpos);
				}
				key = urldecode(key);
				value = urldecode(value);
				cookies.insert(std::make_pair(key, value));
			}
		}
	}
}

//get
void Request::fetch_queries(std::string req_str) {
	size_t pos = 0, kpos = 0, vpos = 0;
	std::string params = "";

	if ((pos = uri.find("?", 0)) != std::string::npos) {
		params = uri.substr(pos+1);
	} else { //no get method
		return ; // TODO: create excpetion
	}
	while ((kpos = params.find("=",vpos)) != std::string::npos){
		std::string key;
		std::string value;
		key = temp.substr(vpos, kpos - vpos);
		if((vpos = temp.find("&",kpos)) != std::string::npos) {
			value = temp.substr(kpos + 1, vpos - kpos - 1);
			vpos = vpos + 1;
		} else {
			vpos = temp.find("\r\n", kpos);
			value = temp.substr(kpos + 1, vpos - kpos);
		}
		key = urldecode(key);
		value = urldecode(value);
		args.insert(std::make_pair(key, value));
	}
}

//post
void Request::fetch_forms(std::string req_str) {
	size_t pos = 0, prev_pos = 0, kpos = 0, vpos = 0;
	std::string data = "";

	// to extract the POST params we first need to find it and it's length
	if ((pos = req_str.find("content-length:", prev_pos)) != std::string::npos) {
		size_t eol = 0;
		int content_length = 0;
		if ((eol = req_str.find("\r\n", pos)) != std::string::npos)
			content_length = std::stoi(req_str.substr(pos + 16, eol - pos + 16));
		// don't read anything, if content data was 0, or wasn't defined at all
		if (content_length > 0 && (pos = req_str.find("\r\n\r\n", eol)) != std::string::npos)
			data = req_str.substr(pos+4);
		else
			return ;

		while ((kpos = params.find("=",vpos)) != std::string::npos){
			std::string key;
			std::string value;
			key = temp.substr(vpos, kpos - vpos);
			if((vpos = temp.find("&", kpos)) != std::string::npos){
				value = temp.substr(kpos + 1, vpos - kpos - 1);
				vpos = vpos + 1;
			} else {
				vpos = temp.find("\r\n", kpos);
				value = temp.substr(kpos + 1, vpos - kpos);
			}
			key = urldecode(key);
			value = urldecode(value);
			forms.insert(std::make_pair(key, value));
		}

	}
}

std::string Request::urldecode(std::string data) {
	CURL *curl = curl_easy_init();
	std::string result;
	result = curl_easy_unescape(curl, (char *)data.c_str(), data.length());
	return result;
}
