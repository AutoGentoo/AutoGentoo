//
// Created by atuser on 1/28/19.
//

#include <autogentoo/api/request_generate.h>
#include <string.h>
#include <autogentoo/endian_convert.h>

ClientRequest* client_request_init(request_t type) {
	ClientRequest* out = malloc (sizeof(ClientRequest));
	out->null_byte = 0;
	out->request_type = type;
	out->arguments = vector_new(sizeof(ClientRequestArgument), VECTOR_ORDERED | VECTOR_REMOVE);
	
	return out;
}

int client_request_add_structure(ClientRequest* req, request_structure_t struct_type, DynamicType* content) {
	if (struct_type >= STRCT_MAX)
		return -1;
	char* template = request_structure_linkage[(int)struct_type - 1];
	DynamicBinary* dyn = dynamic_binary_new(DB_ENDIAN_TARGET_NETWORK);
	if (dynamic_binary_add_quick(dyn, template, content) != DYNAMIC_BIN_OK)
		return -2;
	
	ClientRequestArgument req_arg = {struct_type, dyn->used_size, dynamic_binary_free(dyn)};
	vector_add(req->arguments, &req_arg);
	
	return 0;
}

#define current_add(item, size) \
memcpy(current, item, size); \
current += size;

int client_request_generate(ClientRequest* req, size_t* size, void** out_ptr) {
	(*size) += 1;
	(*size) += sizeof(request_t);
	(*size) += sizeof(request_structure_t); // STRCT_END
	for (int i = 0; i < req->arguments->n; i++) {
		ClientRequestArgument* argument = (ClientRequestArgument*)vector_get(req->arguments, i);
		(*size) += sizeof(request_structure_t);
		(*size) += argument->size;
	}
	
	(*out_ptr) = malloc(*size);
	void* current = (*out_ptr);
	
	*(char*)current = 0;
	current++;
	current_add(&req->request_type, sizeof(int))
	
	for (int i = 0; i < req->arguments->n; i++) {
		ClientRequestArgument* argument = (ClientRequestArgument*)vector_get(req->arguments, i);
		
		current_add(&argument->struct_type, sizeof(int))
		current_add(argument->ptr, argument->size)
	}
	request_structure_t struct_end = STRCT_END;
	current_add(&struct_end, sizeof(request_structure_t))
	
	return (int)(current - (*out_ptr));
}
