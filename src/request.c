//
// Created by atuser on 4/1/18.
//

#include <autogentoo/request.h>
#include <stdlib.h>
#include <string.h>

Request* request_handle (Connection* conn) {
	Request* out = malloc (sizeof (Request));
	
	out->conn = conn;
	
	if (*(char*)conn->request != 0)
		out->protocol = PROT_HTTP;
	else
		out->protocol = PROT_AUTOGENTOO;
	
	return out;
}

struct __http_header {

};

int http_request_parse (Request* req, HTTPRequest* dest) {
	char* splt_1 = strpbrk((char*)req->conn->request, "\t ");
	if (!splt_1)
		return 1;
	
	char* splt_2 = strpbrk(++splt_1, "\t\n\r ");
	if (!splt_2)
		return 1;
	
	*splt_1 = 0;
	*splt_2 = 0;
	
	dest->type = (char*)req->conn->request;
	dest->arg = splt_1;
	dest->version = ++splt_2;
	
	
	
	return 0;
}

response_t request_call (Request* req) {

}