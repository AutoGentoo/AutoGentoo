//
// Created by atuser on 1/28/19.
//

#ifndef AUTOGENTOO_REQUEST_GENERATE_H
#define AUTOGENTOO_REQUEST_GENERATE_H

#include <autogentoo/request.h>

typedef struct __ClientRequest ClientRequest;

struct __ClientRequest {
	char null_byte; // This is
	request_t request_type;
	Vector* types;
	Vector* arguments;
} __attribute__((packed));

ClientRequest* client_request_init(request_t type);
int client_request_add_V(ClientRequest* req, request_structure_t struct_type, ...);
int client_request_add_S(ClientRequest* req, request_structure_t struct_type, RequestData* structure);
int client_send(ClientRequest* req);

#endif //AUTOGENTOO_REQUEST_GENERATE_H
