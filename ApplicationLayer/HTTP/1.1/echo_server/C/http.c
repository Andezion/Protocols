#include "http.h"
#include <string.h>

struct http_responce http_get(const char *url, struct http_request *request) {
    struct http_responce responce = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Content-Type: text/plain\r\nContent-Length: 11\r\n"),
        .body = strdup("Hello World")
    };

    return responce;
}

struct http_responce http_post(const char *url, const char *body, struct http_request *request) {
    struct http_responce responce = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Content-Type: text/plain\r\nContent-Length: 11\r\n"),
        .body = strdup("Hello World")
    };

    return responce;
}

struct http_responce http_put(const char *url, const char *body, struct http_request *request) {
    request->method = PUT;
    request->url = strdup(url);
    request->version = strdup("HTTP/1.1");
    request->hearders = NULL;
    request->body = strdup(body);
    struct http_responce responce;
}

struct http_responce http_delete(const char *url, struct http_request *request) {
    struct http_responce responce = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Content-Type: text/plain\r\nContent-Length: 0\r\n"),
        .body = NULL
    };

    return responce;
}

struct http_responce http_head(const char *url, struct http_request *request) {
    struct http_responce responce = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Content-Type: text/plain\r\nContent-Length: 0\r\n"),
        .body = NULL
    };  

    return responce;
}

struct http_responce http_options(const char *url, struct http_request *request) {
    struct http_responce responce = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Allow: GET, POST, PUT, DELETE, HEAD, OPTIONS, PATCH, TRACE, CONNECT\r\n"),
        .body = NULL
    };

    return responce;
}

struct http_responce http_patch(const char *url, const char *body, struct http_request *request) {
    request->method = PATCH;
    request->url = strdup(url);
    request->version = strdup("HTTP/1.1");
    request->hearders = NULL;
    request->body = strdup(body);
    struct http_responce responce;
}

struct http_responce http_trace(const char *url, struct http_request *request) {
    request->method = TRACE;
    request->url = strdup(url);
    request->version = strdup("HTTP/1.1");
    request->hearders = NULL;
    request->body = NULL;
    struct http_responce responce;
}

struct http_responce http_connect(const char *url, struct http_request *request) {
    request->method = CONNECT;
    request->url = strdup(url);
    request->version = strdup("HTTP/1.1");
    request->hearders = NULL;
    request->body = NULL;
    struct http_responce responce;
}
