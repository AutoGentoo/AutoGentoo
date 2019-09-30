//
// Created by atuser on 1/28/19.
//

#include <autogentoo/api/request_generate.h>
#include <string.h>
#include <autogentoo/endian_convert.h>

ClientRequest* client_request_init(request_t type) {
	ClientRequest* out = malloc (sizeof(ClientRequest));
	out->request_type = type;
	out->arguments = vector_new(VECTOR_ORDERED | VECTOR_REMOVE);
	out->size = 0;
	out->ptr = NULL;
	
	return out;
}

int client_request_add_structure(ClientRequest* req, request_structure_t struct_type, DynamicType* content) {
	if (struct_type >= STRCT_MAX)
		return -1;
	char* template = request_structure_linkage[(int)struct_type - 1];
	DynamicBinary* dyn = dynamic_binary_new(DB_ENDIAN_TARGET_NETWORK);
	if (dynamic_binary_add_quick(dyn, template, content) != DYNAMIC_BIN_OK)
		return -2;
	
	ClientRequestArgument* req_arg = malloc(sizeof(ClientRequestArgument));
	
	req_arg->struct_type = struct_type;
	req_arg->size = dyn->used_size;
	req_arg->ptr = dynamic_binary_free(dyn);
	
	vector_add(req->arguments, req_arg);
	
	return 0;
}

#define current_add(item, size) \
memcpy(current, item, size); \
current += size;

#define current_add_int(integer) { \
	int big_endian = htonl((integer));\
	memcpy(current, &big_endian, sizeof(int)); \
	current += sizeof(int); \
}

int client_request_generate(ClientRequest* req) {
	req->size = 1;
	req->size += sizeof(request_t);
	req->size += sizeof(request_structure_t); // STRCT_END
	for (int i = 0; i < req->arguments->n; i++) {
		ClientRequestArgument* argument = (ClientRequestArgument*)vector_get(req->arguments, i);
		req->size += sizeof(request_structure_t);
		req->size += argument->size;
	}
	
	if (req->ptr)
		free(req->ptr);
	req->ptr = malloc(req->size);
	void* current = req->ptr;
	
	*(char*)current = 0;
	current++;
	current_add_int(req->request_type);
	
	for (int i = 0; i < req->arguments->n; i++) {
		ClientRequestArgument* argument = (ClientRequestArgument*)vector_get(req->arguments, i);
		current_add_int(argument->struct_type)
		current_add(argument->ptr, argument->size)
	}
	request_structure_t struct_end = STRCT_END;
	current_add_int(struct_end)
	
	return (int)(current - req->ptr);
}

void client_request_free(ClientRequest* req) {
	for (int i = 0; i < req->arguments->n; i++) {
		ClientRequestArgument* arg = (ClientRequestArgument*)vector_get(req->arguments, i);
		free(arg->ptr);
		free(arg);
	}
	
	vector_free(req->arguments);
	if (req->ptr)
		free(req->ptr);
	free(req);
}