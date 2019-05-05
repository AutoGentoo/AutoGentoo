//
// Created by atuser on 4/1/18.
//

#include <autogentoo/request.h>
#include <string.h>
#include <unitypes.h>
#include <netinet/in.h>
#include <autogentoo/handle.h>
#include <autogentoo/http.h>

int prv_read_int (void** data, void* end, int* out) {
	if (*data + sizeof (uint32_t) > end)
		return -1;
	
	*out = ntohl(*(uint32_t*)*data);
	*data += sizeof (uint32_t);
	return 0;
}

#define AUTOGENTOO_REQUEST_HANDLE_ERROR {free(out); return NULL;}
#define AUTOGENTOO_REQUEST_HANDLE_VECTOR_ERROR {\
vector_free (out->structures_parent); \
vector_free (out->types_parent); \
AUTOGENTOO_REQUEST_HANDLE_ERROR}

#define AUTOGENTOO_READ_INT(destination) {\
	if (prv_read_int (&current_request, end, (int*)&(destination)) == -1)\
		AUTOGENTOO_REQUEST_HANDLE_ERROR\
};
#define AUTOGENTOO_READ_INT_ERROR(destination, error) {\
	if (prv_read_int (&current_request, end, (int*)&(destination)) == -1){\
		(error);\
	}\
};

Request* request_handle (Connection* conn) {
	Request* out = malloc (sizeof (Request));
	
	out->parent = conn->parent;
	out->conn = conn;
	out->structures = NULL;
	out->structures_parent = NULL;
	out->struct_c = 0;
	out->protocol = PROT_HTTP; // Since vectors are set to NULL we assume HTTP
	
	out->body = conn->request;
	out->length = conn->size;
	
	void* current_request = out->body;
	
	/* If our first byte is a NULL byte, protocol == PROT_AUTOGENTOO */
	if (*(char*)current_request != 0) {
		out->protocol = PROT_HTTP;
		return out;
	}
	else
		out->protocol = PROT_AUTOGENTOO;
	
	current_request++;
	void* end = out->body + out->length;
	
	AUTOGENTOO_READ_INT(out->request_type)
	
	if (out->request_type & REQ_OPT_STREAM) {
		out->request_type &= ~REQ_OPT_STREAM;
		out->directive = DIR_CONNECTION_STREAM;
	}
	
	out->structures_parent = vector_new (sizeof (RequestData), VECTOR_REMOVE | VECTOR_ORDERED);
	out->types_parent = vector_new (sizeof (request_structure_t), VECTOR_REMOVE | VECTOR_ORDERED);
	out->structures = out->structures_parent->ptr;
	out->types = out->types_parent->ptr;
	
	request_structure_t current;
	AUTOGENTOO_READ_INT_ERROR(current, AUTOGENTOO_REQUEST_HANDLE_VECTOR_ERROR)
	
	long d = 0;
	while (current != STRCT_END) {
		if (current >= STRCT_MAX)
			AUTOGENTOO_REQUEST_HANDLE_VECTOR_ERROR
		
		vector_add(out->types_parent, &current);
		vector_add(out->structures_parent, &d);
		out->struct_c++;
		int len = parse_request_structure (
				(RequestData*)vector_get(out->structures_parent, out->struct_c - 1),
				request_structure_linkage[current - 1],
				current_request, end);
		if (len == -1)
			AUTOGENTOO_REQUEST_HANDLE_VECTOR_ERROR
		current_request += len;
		
		AUTOGENTOO_READ_INT_ERROR(current, AUTOGENTOO_REQUEST_HANDLE_VECTOR_ERROR)
	}
	
	return out;
}

void request_call(Response* res, Request* req) {
	if (req->protocol == PROT_AUTOGENTOO) {
		req->resolved_call = resolve_call(req->request_type);
		if (req->resolved_call.ag_fh == NULL)
			HANDLE_RETURN(NOT_IMPLEMENTED);
		return (*req->resolved_call.ag_fh) (res, req);
	}
	
	HttpRequest* http_req = ag_http_parse(req->body);
	
	if (http_req == NULL) {
		http_send_default(req->conn, BAD_REQUEST);
		HANDLE_RETURN(BAD_REQUEST);
	}
	
	http_req->response_headers = small_map_new(5);
	http_add_default_headers(http_req);
	
	if (((HttpHeader*)small_map_get(http_req->headers, "Connection")) != NULL)
		if (strcmp(((HttpHeader*)small_map_get(http_req->headers, "Connection"))->value, "keep-alive") == 0)
			req->directive = DIR_CONNECTION_OPEN;
	req->resolved_call = resolve_call(http_req->function);
	
	if (req->resolved_call.http_fh == NULL) {
		http_send_default(req->conn, NOT_IMPLEMENTED);
		http_free_request(http_req);
		HANDLE_RETURN(NOT_IMPLEMENTED);
	}
	
	(*req->resolved_call.http_fh)(req->conn, http_req);
	res->code = http_req->response;
	http_free_request(http_req);
}

void request_free (Request* req) {
	if (req->protocol == PROT_HTTP)
		return free (req);
	
	for (int i = 0; i != req->struct_c; i++)
		free_request_structure(&req->structures[i],
							   request_structure_linkage[req->types[i] - 1], NULL);
	
	vector_free (req->structures_parent);
	vector_free (req->types_parent);
	free (req);
}
