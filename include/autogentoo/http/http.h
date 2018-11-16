//
// Created by atuser on 2/16/18.
//

#ifndef AUTOGENTOO_HTTP_H
#define AUTOGENTOO_HTTP_H

#include <stdio.h>
#include <autogentoo/hacksaw/tools.h>

typedef struct __HttpHeader HttpHeader;
typedef struct __HttpRequest HttpRequest;

typedef enum {
	HEADER_NOT_FOUND,
	HEADER_FOUND
} http_header_t;

typedef enum {
	HTTP_FUNCTION_NOT_FOUND,
	HTTP_FUNCTION_GET,
	HTTP_FUNCTION_HEAD,
	HTTP_FUNCTION_POST,
} http_function_t;

struct __HttpHeader {
	char* name;
	char* value;
};

struct __HttpRequest {
	http_function_t function;
	char* path;
	int body_start; //!< Start of body on request body, For GET(optional) and POST
	
	struct {
		int maj;
		int min;
	} version;
	
	SmallMap* headers;
	size_t request_size;
};

#include <autogentoo/request.h>

HttpRequest* http_request_parse(Request* req);
HttpHeader* parse_headers(char* str, char** end_str);
char* get_header(HttpHeader* HEAD, char* to_find);
void free_headers (HttpHeader* HEAD);
void free_http_request(HttpRequest* ptr);

#endif //AUTOGENTOO_HTTP_H
