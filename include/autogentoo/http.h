//
// Created by atuser on 2/16/18.
//

#ifndef AUTOGENTOO_HTTP_H
#define AUTOGENTOO_HTTP_H

typedef struct _http_request_header http_request_header;

typedef enum {
	HEADER_NOT_FOUND,
	HEADER_FOUND
} http_header_t;

struct _http_request_header {
	char* name;
	char* value;
	
	struct _http_request_header* next;
};

http_request_header* parse_headers (char* str);
char* get_header(http_request_header* HEAD, char* to_find);
void free_headers (http_request_header* HEAD);

#endif //AUTOGENTOO_HTTP_H
