#include "http.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h> 

// суть get метода - что мы типа просим сервер отдать нам какую-то информацию, которая хранится на сервере
struct http_response http_get(const char *url, struct http_request *request) {
    // тут создаём наш ответ
    struct http_response response;
    
    // в ответе мы скажем плиенту привет
    const char *body = "Hello World";
    size_t body_len = strlen(body);

    // формируем заголовки ответа, указывая тип контента и длину тела ответа
    char headers_buffer[256];
    snprintf(headers_buffer, sizeof(headers_buffer), "Content-Type: text/plain\r\nContent-Length: %zu\r\n", body_len);

    // инициализируем структуру http_response, выделяя память для строк и копируя данные
    memset(&response, 0, sizeof(response));

    // тут у нас версия протокола
    response.version = strdup("HTTP/1.1");
    // тут код нашего ответа 
    response.status_code = HTTP_OK;
    // тут типа сообщение, которое соответствует коду ответа
    response.status_message = strdup("OK");
    // тут заголовки нашего ответа
    response.headers = strdup(headers_buffer);
    // тут тело нашего ответа
    response.body = strdup(body);


    // проверка на хуйню с памятью
    if (!response.version || !response.status_message || !response.headers || !response.body) {
        fprintf(stderr, "Memory allocation failed\n");

        free(response.version);
        free(response.status_message);
        free(response.headers);
        free(response.body);

        response.version = NULL;
        response.status_message = NULL;
        response.headers = NULL;
        response.body = NULL;
    }

    // ну и тут мы возвращаем наш ответ клиенту
    return response;
}

struct http_response http_post(const char *url, const char *body, struct http_request *request) {
    struct http_response response;

    return response;
}

struct http_response http_put(const char *url, const char *body, struct http_request *request) {
    struct http_response response;

    return response;
}

struct http_response http_delete(const char *url, struct http_request *request) {
    struct http_response response = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Content-Type: text/plain\r\nContent-Length: 0\r\n"),
        .body = NULL
    };

    return response;
}

struct http_response http_head(const char *url, struct http_request *request) {
    struct http_response response = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Content-Type: text/plain\r\nContent-Length: 0\r\n"),
        .body = NULL
    };  

    return response;
}

struct http_response http_options(const char *url, struct http_request *request) {
    struct http_response response = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Allow: GET, POST, PUT, DELETE, HEAD, OPTIONS, PATCH, TRACE, CONNECT\r\n"),
        .body = NULL
    };

    return response;
}

struct http_response http_patch(const char *url, const char *body, struct http_request *request) {
    struct http_response response = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Content-Type: text/plain\r\nContent-Length: 11\r\n"),
        .body = strdup("Hello World")
    };

    return response;
}

struct http_response http_trace(const char *url, struct http_request *request) {
    struct http_response response = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Content-Type: text/plain\r\nContent-Length: 0\r\n"),
        .body = NULL
    };

    return response;
}

struct http_response http_connect(const char *url, struct http_request *request) {
    struct http_response response = {
        .version = strdup("HTTP/1.1"),
        .status_code = HTTP_OK,
        .status_message = strdup("OK"),
        .headers = strdup("Content-Type: text/plain\r\nContent-Length: 0\r\n"),
        .body = NULL
    };

    return response;
}
