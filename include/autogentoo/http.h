//
// Created by atuser on 2/16/18.
//

#ifndef AUTOGENTOO_HTTP_H
#define AUTOGENTOO_HTTP_H

#include <stdio.h>
#include <autogentoo/hacksaw/tools.h>

typedef struct __HttpRequest HttpRequest;

#include <autogentoo/request.h>
#include <autogentoo/header.h>

struct __HttpRequest {
	request_t function;
	char* path;
	int body_start; //!< Start of body on request body, For GET(optional) and POST
	
	struct {
		int maj;
		int min;
	} version;
	
	SmallMap* headers;
	size_t request_size;
	
	HttpRequestHeaders request_headers;
	HttpResponseHeaders response_headers;
};


HttpHeader* get_header(HttpRequest* request, char* to_find);
void free_http_request(HttpRequest* ptr);

/* From flex/bison */
HttpRequest* ag_http_parse (char* buffer);

#endif //AUTOGENTOO_HTTP_H
