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
	
	out->arguments = vector_new(sizeof(void*), ORDERED | REMOVE);
	out->types = vector_new(sizeof(request_structure_t), ORDERED | REMOVE);
	
	return out;
}

int client_request_add_V(ClientRequest* req, request_structure_t struct_type, ...) {
	if (struct_type >= STRCT_MAX)
		return -1;
	
	char* template = request_structure_linkage[(int)struct_type];
	
	
	va_list args;
	va_start(args, struct_type);
	
	/* Size check */
	size_t size = 0;
	
	Vector* argument_tree = vector_new(sizeof(void*), ORDERED | KEEP);
	
	for (char* c = template; *c; c++) {
		void* item;
		int item_int;
		if (*c == 'i') {
			item_int = va_arg(args, int);
			
		}
		else if (*c == 's')
		
	}
}

int client_request_add_S(ClientRequest* req, request_structure_t struct_type, RequestData* structure) {

}

int client_send(ClientRequest* req);
