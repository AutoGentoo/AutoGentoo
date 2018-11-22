//
// Created by atuser on 11/20/18.
//

#ifndef AUTOGENTOO_HEADER_H
#define AUTOGENTOO_HEADER_H

#include <autogentoo/request.h>

typedef struct __HttpHeader HttpHeader;
typedef struct __HeaderHandler HeaderHandler;

typedef void (*header_handler_t) (Request* request, HttpHeader* heeader);

struct __HttpHeader {
	char* name;
	char* value;
};

struct __HeaderHandler {
	char* name;
	header_handler_t handler;
};

#endif //AUTOGENTOO_HEADER_H
