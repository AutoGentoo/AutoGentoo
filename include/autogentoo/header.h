//
// Created by atuser on 11/20/18.
//

#ifndef AUTOGENTOO_HEADER_H
#define AUTOGENTOO_HEADER_H

#include <autogentoo/request.h>

typedef struct __HttpHeader HttpHeader;
typedef struct __HeaderHandler HeaderHandler;

typedef void (*header_handler_t) (Request* request, HttpHeader* heeader);

typedef struct __HttpRequestHeaders HttpRequestHeaders;
typedef struct __HttpResponseHeaders HttpResponseHeaders;

typedef enum {
	HTTP_CONNECTION_CLOSE,
	HTTP_CONNECTION_KEEP_ALIVE
	
} http_header_t;

struct __HttpRequestHeaders {
	char* Accept_Encoding;
	http_header_t Connection;
	size_t Content_Length; //!< sizeof (body)
	char* Host;
	struct {
		char* unit;
		struct {
			int start;
			int finish;
		} *ranges;
	} Range; //!< bytes=s-f
	char* User_Agent;
	char* Upgrade_Insecure_Requests;
	
};

struct __HttpResponseHeaders {
	char* Cache_Control;
	char* Connection;
	char* Content_Encoding;
	char* Content_Length;
	char* Content_Range; //!< bytes s-f/total
	char* Location;
	char* Server;
	char* Vary;
};

struct __HttpHeader {
	char* name;
	char* value;
};



struct __HeaderHandler {
	char* name;
	header_handler_t handler;
};



#endif //AUTOGENTOO_HEADER_H
