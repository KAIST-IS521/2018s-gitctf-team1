#ifndef SRC_REQUEST_H_
#define SRC_REQUEST_H_
#include <map>

class request {
    public:
        bool valid;
        string method;
        string uri;
        string http_version;

        std::map<string, string> args; // GET_PARAMETERS
        std::map<string, string> forms; // POST_PARAMETERS
        std::map<string, string> cookies; // COOKIES

    private:
        void fetch_headers(string req_str);
        void fetch_cookies(string req_str);
        void fetch_queries(string req_str);
        void fetch_forms(string req_str);
	string urldecode(string data);
};
#endif

