#pragma once 

enum http_method {
    GET,
    POST,
    PUT,
    DELETE,
    UNKNOWN
};

struct http_request {
    enum http_method method;
    char path[256];
    char version[16];
    char body[1024];
};