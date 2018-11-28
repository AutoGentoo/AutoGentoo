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
	
	out->conn = conn;
	out->structures = NULL;
	out->structures_parent = NULL;
	out->struct_c = 0;
	out->protocol = PROT_HTTP; // Since vectors are set to NULL we assume HTTP
	
	out->body = conn->request;
	out->length = conn->size;
	
	void* current_request = out->body;
	
	if (*(char*)current_request != 0)
		out->protocol = PROT_HTTP;
	else {
		out->protocol = PROT_AUTOGENTOO;
		current_request++;
	}
	
	if (out->protocol == PROT_HTTP)
		return out;
	
	void* end = out->body + out->length;
	
	AUTOGENTOO_READ_INT(out->request_type);
	
	out->structures_parent = vector_new (sizeof (RequestData), REMOVE | ORDERED);
	out->types_parent = vector_new (sizeof (request_structure_t), REMOVE | ORDERED);
	out->structures = out->structures_parent->ptr;
	out->types = out->types_parent->ptr;
	
	request_structure_t current;
	AUTOGENTOO_READ_INT_ERROR(current, AUTOGENTOO_REQUEST_HANDLE_VECTOR_ERROR);
	
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
		
		AUTOGENTOO_READ_INT_ERROR(current, AUTOGENTOO_REQUEST_HANDLE_VECTOR_ERROR);
	}
	
	return out;
}

response_t request_call (Request* req) {
	if (req->protocol == PROT_AUTOGENTOO) {
		req->resolved_call = resolve_call(req->request_type);
		if (req->resolved_call.ag_fh == NULL)
			return NOT_IMPLEMENTED;
		return (*req->resolved_call.ag_fh) (req);
	}
	
	HttpRequest* http_req = ag_http_parse(req->body);
	
	if (http_req == NULL) {
		http_send_bad_request(req->conn);
		return BAD_REQUEST;
	}
	
	http_add_default_headers(http_req);
	
	int i;
	req->resolved_call = resolve_call(http_req->function);
	
	if (req->resolved_call.http_fh == NULL) {
		http_req->response = NOT_IMPLEMENTED;
		http_send_headers(req->conn, http_req);
		return http_req->response;
	}
	
	(*req->resolved_call.http_fh)(req->conn, http_req);
	http_free_request(http_req);
	
	return http_req->response;
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