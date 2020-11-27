//
// Created by atuser on 2/16/18.
//

#ifndef AUTOGENTOO_HTTP_H
#define AUTOGENTOO_HTTP_H

#include <stdio.h>
#include <autogentoo/hacksaw/hacksaw.h>

typedef struct __HttpRequest HttpRequest;
typedef struct __HttpHeader HttpHeader;

#include <autogentoo/request.h>
#include "user.h"

struct __HttpHeader {
    char* name;
    char* value;
};

struct __HttpRequest {
    request_t function;
    char* path;
    int body_start; //!< Start of body on request body, For GET(optional) and POST

    struct {
        int maj;
        int min;
    } version;

    SmallMap* headers;
    SmallMap* response_headers;
    response_t response;

    size_t request_size;

    //HttpRequestHeaders request_headers;
    //HttpResponseHeaders response_headers;
};


HttpHeader* http_get_header(HttpRequest* request, char* to_find);

void http_free_request(HttpRequest* ptr);

int http_check_authorization(Server* parent, HttpRequest* request, char* host_id, token_access_t access_level);

void http_add_header(HttpRequest* req, char* name, char* value);

void http_add_default_headers(HttpRequest* req);

FILE* http_handle_path(Server* parent, HttpRequest* req, long* size);

size_t http_send_headers(Connection* conn, HttpRequest* req);

ssize_t http_send_default(Connection* conn, response_t res);

/* From flex/bison */
HttpRequest* ag_http_parse(char* buffer);

#endif //AUTOGENTOO_HTTP_H
