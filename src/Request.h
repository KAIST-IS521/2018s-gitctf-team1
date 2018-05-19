#pragma once

#include <string>
#include <map>

class Request {
    private:
        bool valid;
        std::string method;
        std::string uri;
        std::string http_version;
        std::string raw_data;

        std::map<std::string, std::string> header;
        std::map<std::string, std::string> get; // GET_PARAMETERS
        std::map<std::string, std::string> post; // POST_PARAMETERS
        std::map<std::string, std::string> cookie; // COOKIES

        void fetch_headers(std::string req_str);
        void fetch_cookies();
        void fetch_queries();
        void fetch_forms();
        std::string getParameters(std::map<std::string, std::string> target, char delimiter);

    public:
        Request(std::string req_str);
        std::string getHeader(std::string key);
        std::string getQueryString();
        std::string getPostData();
        std::string getCookie();
        virtual ~Request();
};
