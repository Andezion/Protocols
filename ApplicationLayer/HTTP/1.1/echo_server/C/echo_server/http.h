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
    char *url;
    char *version;
    char *hearders;
    char *body;
};