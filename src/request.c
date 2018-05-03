//
// Created by atuser on 4/1/18.
//

#include <autogentoo/request.h>
#include <string.h>
#include <unitypes.h>
#include <netinet/in.h>
#include <autogentoo/handle.h>

int prv_read_int (void** data, void* end, int* out) {
	if (*data + sizeof (uint32_t) > end)
		return -1;
	
	*out = ntohl(*(uint32_t*)*data);
	*data += sizeof (uint32_t);
	return 0;
}

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
	
	void* end = out->conn->request + out->conn->size;
	if (prv_read_int (&current_request, end, (int*)&out->request_type) == -1) {
		free (out);
		return NULL;
	}
	
	out->structures = vector_new (sizeof (RequestData), REMOVE | ORDERED);
	out->types = vector_new (sizeof (request_structure_t), REMOVE | ORDERED);
	
	request_structure_t current;
	if (prv_read_int (&current_request, end, (int*)&current) == -1) {
		vector_free (out->structures);
		vector_free (out->types);
		free (out);
		return NULL;
	}
	
	long d = 0;
	while (current != STRCT_END) {
		vector_add(out->types, &current);
		vector_add(out->structures, &d);
		out->struct_c++;
		int len = parse_request_structure (
				(RequestData*)vector_get(out->structures, out->struct_c - 1),
				request_structure_linkage[current - 1],
				current_request, end);
		if (len == -1) {
			request_free (out);
			return NULL;
		}
		current_request += len;
		
		if (prv_read_int (&current_request, end, (int*)&current) == -1) {
			request_free (out);
			return NULL;
		}
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
		free_request_structure(&(((RequestData*)req->structures->ptr)[i]),
							   request_structure_linkage[((request_structure_t*)req->types)[i]], NULL);
	
	vector_free (req->structures);
	vector_free (req->types);
	free (req);
}