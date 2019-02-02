//
// Created by atuser on 1/28/19.
//

#ifndef AUTOGENTOO_REQUEST_GENERATE_H
#define AUTOGENTOO_REQUEST_GENERATE_H

#include <autogentoo/request.h>

typedef struct __ClientRequest ClientRequest;
typedef struct __ClientRequestArgument ClientRequestArgument;
typedef union __ClientType ClientType;

struct __ClientRequest {
	char null_byte; // This is
	request_t request_type;
	Vector* arguments;
};

struct __ClientRequestArgument {
	request_structure_t struct_type;
	size_t size;
	void* ptr;
};

typedef enum {
	CLIENT_TYPE_ARRAY_NEXT = -1,
	CLIENT_TYPE_ARRAY_END = -2
} ClientType_array_t;

union __ClientType {
	char* string;
	int integer;
	struct {
		void* data;
		size_t n;
	} binary;
	
	ClientType_array_t array_directive;
};


ClientRequest* client_request_init(request_t type);
int client_request_add_structure(ClientRequest* req, request_structure_t struct_type, ClientType* content);
int client_request_generate(ClientRequest* req, size_t* size, void** out_ptr);

#endif //AUTOGENTOO_REQUEST_GENERATE_H
