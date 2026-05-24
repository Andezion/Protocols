#include "http.h"

http_responce http_get(const char *url, struct http_request *request);
http_responce http_post(const char *url, const char *body, struct http_request *request);
http_responce http_put(const char *url, const char *body, struct http_request *request);
http_responce http_delete(const char *url, struct http_request *request);
http_responce http_head(const char *url, struct http_request *request);
http_responce http_options(const char *url, struct http_request *request);
http_responce http_patch(const char *url, const char *body, struct http_request *request);
http_responce http_trace(const char *url, struct http_request *request);
http_responce http_connect(const char *url, struct http_request *request);
