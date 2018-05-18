#ifndef SRC_RESPONSE_H_
#define SRC_RESPONSE_H_
#include <map>
// HTTP STATUS CODE
#define HTTP_OK                 200
#define HTTP_BAD_REQUEST        400
#define HTTP_FORBIDDEN          403
#define HTTP_NOT_FOUND          404
#define HTTP_UNSUP_MEDIA_TYPE   415
#define HTTP_INTERNAL_SRV_ERROR 500
#define HTTP_NOT_IMPLEMENTED    501

class response {
    public:
        std::map<string, string> headers;
        string body;
        bool has_headers;

        // XXX: change prototype
        response();
        // rendering response
        string render();
};

#endif
