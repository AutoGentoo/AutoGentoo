//
// Created by atuser on 4/1/18.
//

#include <autogentoo/request.h>
#include <stdlib.h>
#include <string.h>
#include <unitypes.h>
#include <netinet/in.h>
#include <autogentoo/handle.h>

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
	out->request_type = (request_t)ntohl((uint32_t) out->request_type);
	current_request += sizeof (request_t);
	
	memcpy (&out->struct_c, current_request, sizeof (request_t));
	out->struct_c = ntohl((uint32_t) out->struct_c);
	
	current_request += sizeof (int);
	
	out->structures = malloc (sizeof (RequestData) * out->struct_c);
	out->types = malloc (sizeof (request_structure_t) * out->struct_c);
	
	request_structure_t current;
	memcpy(&current, current_request, sizeof (request_structure_t));
	current = (request_structure_t)ntohl((uint32_t) current);
	
	current_request += sizeof(request_t);
	int len = 0;
	int i = 0;
	
	void* end = out->conn->request + out->conn->size;
	
	for (; i < out->struct_c && current != STRCT_END; i++) {
		len = parse_request_structure (&out->structures[i], request_structure_linkage[out->types[i]], current_request, end);
		if (len == -1) {
			request_free (out);
			return NULL;
		}
		current_request += len;
		memcpy(&current, current_request, sizeof (request_structure_t));
		current = (request_structure_t)ntohl((uint32_t) current);
		out->types[i] = current;
		current_request += sizeof (request_structure_t);
	}
	
	return out;
}

struct __http_header {

};

int http_request_parse (Request* req, HTTPRequest* dest) {
	char* splt_1 = strpbrk((char*)req->conn->request, "\t ");
	if (!splt_1)
		return 1;
	
	char* splt_2 = strpbrk(splt_1 + 1, "\t\n\r ");
	if (!splt_2)
		return 1;
	
	char* splt_3 = strpbrk(splt_2 + 1, "\t\n\r ");
	if (!splt_3)
		return 1;
	
	
	dest->function = strndup((char*)req->conn->request, splt_1 - (char*)req->conn->request);
	dest->arg = strndup(splt_1 + 1, splt_2 - (splt_1 + 1));
	dest->version = strndup (splt_2 + 1, splt_3 - (splt_2 + 1));
	
	return 0;
}

response_t request_call (Request* req) {
	req->resolved_call = resolve_call(req->request_type);
	
	if (req->resolved_call.ag_fh == NULL)
		return NOT_IMPLEMENTED;
	if (req->protocol == PROT_AUTOGENTOO)
		return (*req->resolved_call.ag_fh) (req);
	
	HTTPRequest http_req;
	if (http_request_parse (req, &http_req) != 0)
		return BAD_REQUEST;
	return (*req->resolved_call.http_fh)(req->conn, http_req);
}

void request_free (Request* req) {
	for (int i = 0; i != req->struct_c; i++)
		free_request_structure(&req->structures[i], request_structure_linkage[req->types[i]], NULL);
	
	free (req->structures);
	free (req->types);
	free (req);
}