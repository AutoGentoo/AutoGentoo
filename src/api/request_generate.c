//
// Created by atuser on 1/28/19.
//

#include <autogentoo/api/request_generate.h>
#include <autogentoo/dynamic_binary.h>
#include <string.h>
#include <autogentoo/endian_convert.h>

ClientRequest* client_request_init(request_t type) {
	ClientRequest* out = malloc (sizeof(ClientRequest));
	out->null_byte = 0;
	out->request_type = type;
	out->arguments = vector_new(sizeof(ClientRequestArgument), ORDERED | REMOVE);
	
	return out;
}

int client_request_add_structure(ClientRequest* req, request_structure_t struct_type, ClientType* content) {
	if (struct_type >= STRCT_MAX)
		return -1;
	char* template = request_structure_linkage[(int)struct_type - 1];
	
	
	DynamicBinary* dyn = dynamic_binary_new();
	int i = 0;
	
	struct array_pos_node {
		struct array_pos_node* parent_array;
		char* pos;
	} *array_return = NULL;
	
	for (char* c = template; *c; c++) {
		void* item = NULL;
		if (*c == 'i') {
			content[i].integer = (int)htonl((uint32_t)content[i].integer);
			item = &content[i++].integer;
		}
		else if (*c == 's')
			item = content[i++].string;
		else if (*c == 'v') {
			item = content[i].binary.data;
			dynamic_binary_add_binary(dyn, content[i++].binary.n, item);
			continue;
		}
		else if (*c == 'a') {
			struct array_pos_node* parent = array_return;
			array_return = malloc(sizeof(struct array_pos_node));
			array_return->parent_array = parent;
			
			array_return->pos = ++c;
			dynamic_binary_array_start(dyn);
			continue;
		}
		else if (*c == ')') {
			if (content->array_directive == CLIENT_TYPE_ARRAY_NEXT) {
				if (!array_return) {
					lerror("Invalid template syntax\nToo many array closes");
					return -2;
				}
				
				c = array_return->pos;
				dynamic_binary_array_next(dyn);
				continue;
			}
			else if (content->array_directive == CLIENT_TYPE_ARRAY_END) {
				if (!array_return) {
					lerror("Invalid template syntax\nToo many array closes");
					return -2;
				}
				
				struct array_pos_node* parent = array_return->parent_array;
				free(array_return);
				array_return = parent;
				dynamic_binary_array_end(dyn);
				continue;
			}
		}
		
		if (!item) {
			lerror("Template type not supported '%c'", *c);
			return -3;
		}
		
		dynamic_binary_add(dyn, *c, item);
	}
	
	ClientRequestArgument req_arg = {struct_type, dyn->used_size, dynamic_binary_free(dyn)};
	vector_add(req->arguments, &req_arg);
	
	return 0;
}

#define current_add(item, size) \
memcpy(current, item, size); \
current += size;

#define current_add_int(item) \
{int big_endian = (int)htonl((uint32_t)item); current_add(&big_endian, sizeof(int))}

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
	current_add_int(req->request_type)
	
	for (int i = 0; i < req->arguments->n; i++) {
		ClientRequestArgument* argument = (ClientRequestArgument*)vector_get(req->arguments, i);
		
		current_add_int(argument->struct_type)
		current_add(argument->ptr, argument->size)
	}
	request_structure_t struct_end = STRCT_END;
	current_add(&struct_end, sizeof(request_structure_t))
	
	return (int)(current - (*out_ptr));
}
