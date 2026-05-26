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
    char *headers;
    char *body;
};

struct http_response {
    char *version;
    int status_code;
    char *status_message;
    char *headers;
    char *body;
};

struct http_response http_get(const char *url, struct http_request *request);
struct http_response http_post(const char *url, const char *body, struct http_request *request);
struct http_response http_put(const char *url, const char *body, struct http_request *request);
struct http_response http_delete(const char *url, struct http_request *request);
struct http_response http_head(const char *url, struct http_request *request);
struct http_response http_options(const char *url, struct http_request *request);
struct http_response http_patch(const char *url, const char *body, struct http_request *request);
struct http_response http_trace(const char *url, struct http_request *request);
struct http_response http_connect(const char *url, struct http_request *request);

void http_response_free(struct http_response *response);

#endif
