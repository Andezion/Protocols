#pragma once 

#include "../../../../../../TransportLayer/TCP/echo_server/C/tcp.h"

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

