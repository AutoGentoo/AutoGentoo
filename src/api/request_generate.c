//
// Created by atuser on 1/28/19.
//

#include <autogentoo/api/request_generate.h>

struct __ClientRequestArgument_intermediate {
	request_structure_t struct_type;
	size_t size;
	void* ptr;
};

ClientRequest* client_request_init(request_t type) {
	ClientRequest* out = malloc (sizeof(ClientRequest));
	out->null_byte = 0;
	out->request_type = type;
	
	
}

int client_request_add(request_structure_t struct_type, ...);
