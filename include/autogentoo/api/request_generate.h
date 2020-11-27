//
// Created by atuser on 1/28/19.
//

#ifndef AUTOGENTOO_REQUEST_GENERATE_H
#define AUTOGENTOO_REQUEST_GENERATE_H

#include <autogentoo/request.h>
#include <autogentoo/api/dynamic_binary.h>

typedef struct __ClientRequest ClientRequest;
typedef struct __ClientRequestArgument ClientRequestArgument;

struct __ClientRequest {
    request_t request_type;
    Vector* arguments;

    /* After you run generate */
    size_t size;
    void* ptr;
};

struct __ClientRequestArgument {
    request_structure_t struct_type;
    size_t size;
    void* ptr;
};

ClientRequest* client_request_init(request_t type);

int client_request_add_structure(ClientRequest* req, request_structure_t struct_type, DynamicType* content);

int client_request_generate(ClientRequest* req);

void client_request_free(ClientRequest* req);

#endif //AUTOGENTOO_REQUEST_GENERATE_H
