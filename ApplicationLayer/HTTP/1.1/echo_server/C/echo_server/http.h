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

struct http_responce {
    char *version;
    int status_code;
    char *status_message;
    char *headers;
    char *body;
};

