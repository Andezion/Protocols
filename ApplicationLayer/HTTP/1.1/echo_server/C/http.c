#include "http.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h> 

void http_response_free(struct http_response *r) {
    if (!r) return;
    free(r->version);
    free(r->status_message);
    free(r->headers);
    free(r->body);
    r->version = NULL;
    r->status_message = NULL;
    r->headers = NULL;
    r->body = NULL;
    r->status_code = 0;
}

struct http_response http_get(const char *url, struct http_request *request) {
    struct http_response response;
    const char *body = "Hello World";
    size_t body_len = strlen(body);
    char headers_buffer[256];

    int written = snprintf(headers_buffer, sizeof(headers_buffer),
                           "Content-Type: text/plain\r\nContent-Length: %zu\r\n",
                           body_len);
    if (written < 0) {
        fprintf(stderr, "snprintf failed\n");
        memset(&response, 0, sizeof(response));
        response.status_code = HTTP_INTERNAL_SERVER_ERROR;
        return response;
    }

    memset(&response, 0, sizeof(response));

    response.version = strdup("HTTP/1.1");
    response.status_code = HTTP_OK;
    response.status_message = strdup("OK");
    response.headers = strdup(headers_buffer);
    response.body = strdup(body);

    if (!response.version || !response.status_message || !response.headers || !response.body) {
        fprintf(stderr, "Memory allocation failed\n");
        http_response_free(&response);
        response.status_code = HTTP_INTERNAL_SERVER_ERROR;
    }

    return response;
}

struct http_response http_post(const char *url, const char *body, struct http_request *request) {
    struct http_response response;
    const char *body_src = body ? body : "";
    size_t body_len = strlen(body_src);

    char headers_buffer[256];
    int written = snprintf(headers_buffer, sizeof(headers_buffer),
                           "Content-Type: text/plain\r\nContent-Length: %zu\r\n",
                           body_len);
    if (written < 0) {
        fprintf(stderr, "snprintf failed\n");
        memset(&response, 0, sizeof(response));
        response.status_code = HTTP_INTERNAL_SERVER_ERROR;
        return response;
    }

    memset(&response, 0, sizeof(response));
    response.version = strdup("HTTP/1.1");
    response.status_code = HTTP_OK;
    response.status_message = strdup("OK");
    response.headers = strdup(headers_buffer);
    response.body = strdup(body_src);

    if (!response.version || !response.status_message || !response.headers || !response.body) {
        fprintf(stderr, "Memory allocation failed\n");
        http_response_free(&response);
        response.status_code = HTTP_INTERNAL_SERVER_ERROR;
    }

    return response;
}

struct http_response http_put(const char *url, const char *body, struct http_request *request) {
    struct http_response response;

    return response;
}

struct http_response http_delete(const char *url, struct http_request *request) {
    struct http_response response;

    return response;
}

struct http_response http_head(const char *url, struct http_request *request) {
    struct http_response response;

    return response;
}

struct http_response http_options(const char *url, struct http_request *request) {
    struct http_response response;

    return response;
}

struct http_response http_patch(const char *url, const char *body, struct http_request *request) {
    struct http_response response;

    return response;
}

struct http_response http_trace(const char *url, struct http_request *request) {
    struct http_response response;

    return response;
}

struct http_response http_connect(const char *url, struct http_request *request) {
    struct http_response response;

    return response;
}
