#include "Request.h"
#include <algorithm>
#include <string>
#include <curl/curl.h>
#include <string.h>
#include <iostream>
#include <functional>
#include <cctype>
#include <locale>

// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

std::string &strtolower(std::string &str) {
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
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

std::string Request::getHeader(std::string key){
	return header.find(strtolower(key)) == header.end() ? "": urlencode(header[key]);
}

std::string Request::getParameters(std::map<std::string, std::string> target){
	std::map<std::string, std::string>::iterator iter;
	std::string result;

	for(iter = target.begin(); iter != target.end(); iter++)
		result += iter->first + "=" + iter->second + "&";
	
	return result = urlencode(result.substr(0, result.length()-1));
}

std::string Request::getQueryString(){
	return getParameters(get);
}

std::string Request::getPostData(){
	return getParameters(post);
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
	valid = (
    (http_version == "HTTP/1.0" || http_version == "HTTP/1.1") &&
		(method == "GET" || method == "POST") &&
		(uri.length() > 0)
  );

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
}


void Request::fetch_cookies(std::string req_str) {
	size_t pos = 0, eol = 0, kpos= 0, vpos = 0;

	std::string l_cookies;
	// to extract the POST params we first need to find it and it's length
	if ((pos = req_str.find("cookie: ")) != std::string::npos) {
		if ((eol = req_str.find("\r\n", pos)) != std::string::npos) {
			l_cookies = req_str.substr(pos + 8, eol - pos - 8);

			while ((kpos = l_cookies.find("=",vpos))!=std::string::npos) {
				std::string key;
				std::string value;
				key = l_cookies.substr(vpos, kpos - vpos);

				if ((vpos = l_cookies.find(";", kpos)) != std::string::npos) {
					value = l_cookies.substr(kpos + 1, vpos - kpos - 1);
					vpos = vpos + 2;
				} else {
					vpos = l_cookies.find("\r\n",kpos);
					value = l_cookies.substr(kpos + 1,vpos - kpos);
				}
				key = urldecode(key);
				value = urldecode(value);
				cookie.insert(std::make_pair(strtolower(key), value));
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
		key = params.substr(vpos, kpos - vpos);
		if((vpos = params.find("&",kpos)) != std::string::npos) {
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

		while ((kpos = data.find("=",vpos)) != std::string::npos){
			std::string key;
			std::string value;
			key = data.substr(vpos, kpos - vpos);
			if((vpos = data.find("&", kpos)) != std::string::npos){
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

// https://www.joinc.co.kr/w/Site/Code/C/urlencode

#define IS_ALNUM(ch) ( \
  ( ch >= 'a' && ch <= 'z' ) || \
  ( ch >= 'A' && ch <= 'Z' ) || \
  ( ch >= '0' && ch <= '9' ) || \
  ( ch >= '-' && ch <= '.' ) )

#define IS_HEX(ch) ( \
  ( ch >= '0' && ch <= '9' ) || \
  ( ch >= 'A' && ch <= 'F' ) || \
  ( ch >= 'a' && ch <= 'f' ))

std::string Request::urldecode(std::string str){
  int len;
  std::string ret;
  char hex[3] = {0,};

  len = str.length();
  for (int i = 0; i < len; ++i) {
    if ( str[i] != '%' )
      ret += str[i];
    else {
      if (IS_HEX(str[i + 1]) && IS_HEX(str[i + 2]) && i < (len - 2)) {
        hex[0] = str[i + 1];
        hex[1] = str[i + 2];
        ret += ::strtol( hex, NULL, 16 );

        i += 2;
      } else {
        ret.clear();
        // invalid encoding
      }
    }
  }

  return ret;
}
