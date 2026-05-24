#ifndef HTTP_H
#define HTTP_H

#include "../../../../../TransportLayer/TCP/echo_server/C/tcp.h"

enum http_method {
    GET,
    POST,
    PUT,
    DELETE,
    HEAD,
    OPTIONS,
    PATCH,
    TRACE,
    CONNECT
};

enum http_status_code {
    HTTP_OK = 200,
    HTTP_BAD_REQUEST = 400,
    HTTP_NOT_FOUND = 404,
    HTTP_INTERNAL_SERVER_ERROR = 500
};

struct http_request {
    enum http_method method;
    char *url;
    char *version;
    char *hearders;
    char *body;
};

struct http_responce {
    char *version;
    int status_code;
    char *status_message;
    char *headers;
    char *body;
};

http_responce http_get(const char *url, struct http_request *request);
http_responce http_post(const char *url, const char *body, struct http_request *request);
http_responce http_put(const char *url, const char *body, struct http_request *request);
http_responce http_delete(const char *url, struct http_request *request);
http_responce http_head(const char *url, struct http_request *request);
http_responce http_options(const char *url, struct http_request *request);
http_responce http_patch(const char *url, const char *body, struct http_request *request);
http_responce http_trace(const char *url, struct http_request *request);
http_responce http_connect(const char *url, struct http_request *request);

#endif
