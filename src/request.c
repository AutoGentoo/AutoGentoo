//
// Created by atuser on 4/1/18.
//

#include <autogentoo/request.h>
#include <stdlib.h>
#include <string.h>

Request* request_handle (Connection* conn) {
	Request* out = malloc (sizeof (Request));
	
	out->conn = conn;
	out->structures = NULL;
	out->struct_c = 0;
	
	void* current_request = conn->request;
	
	if (*(char*)current_request != 0)
		out->protocol = PROT_HTTP;
	else {
		out->protocol = PROT_AUTOGENTOO;
		current_request++;
	}
	
	if (out->protocol == PROT_HTTP)
		return out;
	
	memcpy (&out->request_type, current_request, sizeof (request_t));
	current_request += sizeof (request_t);
	
	memcpy (&out->struct_c, current_request, sizeof (request_t));
	current_request += sizeof (int);
	
	out->structures = malloc (sizeof (RequestData) * out->struct_c);
	out->types = malloc (sizeof (request_structure_t) * out->struct_c);
	
	request_structure_t current;
	memcpy(&current, current_request, sizeof (request_structure_t));
	current_request += sizeof(request_t);
	int len = 0;
	int i = 0;
	
	void* end = out->conn->request + out->conn->size;
	
	for (; i < out->struct_c && current != STRCT_END; current_request += sizeof(request_t), i++) {
		out->types[i] = current;
		len = parse_request_structure (&out->structures[i], current, current_request, end);
		if (len == -1) {
			request_free (out);
			return NULL;
		}
		current_request += len;
	}
	
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
	
	dest->function = (char*)req->conn->request;
	dest->arg = splt_1;
	dest->version = ++splt_2;
	
	return 0;
}

response_t request_call (Request* req) {

}

void request_free (Request* req) {
	for (int i = 0; i != req->struct_c; i++)
		free_request_structure(&req->structures[i], req->types[i]);
	
	free (req->structures);
	free (req->types);
	free (req);
}