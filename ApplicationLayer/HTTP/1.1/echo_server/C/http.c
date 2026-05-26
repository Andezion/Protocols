#include "http.h"
#include <string.h>
#include <stdio.h>

// суть get метода - что мы типа просим сервер отдать нам какую-то информацию, которая хранится на сервере
struct http_response http_get(const char *url, struct http_request *request) {
    struct http_response response;
    
    const char *body = "Hello World";
    size_t body_len = strlen(body);

    char headers_buffer[256];
    snprintf(headers_buffer, sizeof(headers_buffer), "Content-Type: text/plain\r\nContent-Length: %zu\r\n", body_len);

    memset(&response, 0, sizeof(response));

    response.version = strdup("HTTP/1.1");
    response.status_code = HTTP_OK;
    response.status_message = strdup("OK");
    response.headers = strdup(headers_buffer);
    response.body = strdup(body);


    return response;
}

struct http_response http_post(const char *url, const char *body, struct http_request *request) {
    struct http_response responce = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Content-Type: text/plain\r\nContent-Length: 11\r\n"),
        .body = strdup("Hello World")
    };

    return responce;
}

struct http_response http_put(const char *url, const char *body, struct http_request *request) {
    struct http_response responce = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Content-Type: text/plain\r\nContent-Length: 11\r\n"),
        .body = strdup("Hello World")
    };

    return responce;
}

struct http_response http_delete(const char *url, struct http_request *request) {
    struct http_response responce = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Content-Type: text/plain\r\nContent-Length: 0\r\n"),
        .body = NULL
    };

    return responce;
}

struct http_response http_head(const char *url, struct http_request *request) {
    struct http_response responce = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Content-Type: text/plain\r\nContent-Length: 0\r\n"),
        .body = NULL
    };  

    return responce;
}

struct http_response http_options(const char *url, struct http_request *request) {
    struct http_response responce = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Allow: GET, POST, PUT, DELETE, HEAD, OPTIONS, PATCH, TRACE, CONNECT\r\n"),
        .body = NULL
    };

    return responce;
}

struct http_response http_patch(const char *url, const char *body, struct http_request *request) {
    struct http_response responce = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Content-Type: text/plain\r\nContent-Length: 11\r\n"),
        .body = strdup("Hello World")
    };

    return responce;
}

struct http_response http_trace(const char *url, struct http_request *request) {
    struct http_response responce = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Content-Type: text/plain\r\nContent-Length: 0\r\n"),
        .body = NULL
    };

    return responce;
}

struct http_response http_connect(const char *url, struct http_request *request) {
    struct http_response responce = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Content-Type: text/plain\r\nContent-Length: 0\r\n"),
        .body = NULL
    };

    return responce;
}
