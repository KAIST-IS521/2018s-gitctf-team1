#include "Request.h"
#include <algorithm>
#include <curl/curl.h>
using namespace std;
request::request(string req_str) {
	std::transform(req_str.begin(), req_str.end(),
			req_str.begin(), std::tolower);
	fetch_headers(req_str);
	if (valid) {
		fetch_cookies(req_str);
		fetch_queries(req_str); //get
		fetch_forms(req_str); //post
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


void request::fetch_cookies(string req_str) {
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
				key=urlencode(key);
				value=urlencode(value);
				cookies.insert(make_pair(key,value));
			}
		}
	}
}

//get
void request::fetch_queries(string req_str) {
	size_t pos = 0, kpos = 0, vpos = 0;
	std::string params = "";

	if ((pos = uri.find("?", 0)) != std::string::npos) {
		params = uri.substr(pos+1);
	}
	else { //no get method
		return ;
	}
	while((kpos = params.find("=",vpos))!=std::string::npos){
		std::string key="";
		std::string value="";
		key = temp.substr(vpos,kpos-vpos);
		if((vpos = temp.find("&",kpos))!=std::string::npos){
			value = temp.substr(kpos+1,vpos-kpos-1);
			vpos = vpos+1;	
		}
		else{
			vpos = temp.find("\r\n",kpos);
			value = temp.substr(kpos+1,vpos-kpos);
		}
		key=urlencode(key);
		value=urlencode(value);
		args.insert(make_pair(key,value));
	}
}

//post
void request::fetch_forms(string req_str) {
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

		while((kpos = params.find("=",vpos))!=std::string::npos){
			std::string key="";
			std::string value="";
			key = temp.substr(vpos,kpos-vpos);
			if((vpos = temp.find("&",kpos))!=std::string::npos){
				value = temp.substr(kpos+1,vpos-kpos-1);
				vpos = vpos+1;	
			}
			else{
				vpos = temp.find("\r\n",kpos);
				value = temp.substr(kpos+1,vpos-kpos);
			}
			key=urlencode(key);
			value=urlencode(value);
			forms.insert(make_pair(key,value));
		}

	}
}
string request::urlencode(string data)
{
	CURL *curl = curl_easy_init();
	std::string result="";

	result = curl_easy_escape(curl,(char*)data.c_str(),data.length());

	return result;
}

string request::urldecode(string data)
{
	CURL *curl = curl_easy_init();
	std::string result="";

	result = curl_easy_unescape(curl,(char*)data.c_str(),data.length());

	return result;
}
