//
// Created by atuser on 4/1/18.
//

#ifndef AUTOGENTOO_REQUEST_H
#define AUTOGENTOO_REQUEST_H

#include "response.h"

typedef struct __Request Request;
typedef struct __HTTPRequest HTTPRequest;

/**
 * A function pointer for to handle requests
 * @param conn the connection of the request
 * @param args a list of arguments passed to the handler
 * @param the index to start reading the request at
 */
typedef response_t (* HTTP_FH)(Connection* conn, HTTPRequest req);
typedef response_t (* AUTOGENTOO_FH) (Request* request);

typedef union __RequestData RequestData;

typedef enum {
	PROT_AUTOGENTOO = 0, // guarentees first byte is 0 (cant be HTTP)
	PROT_HTTP
} protocol_t;

typedef enum {
	REQ_GET,
	REQ_INSTALL,
	REQ_EDIT,
	REQ_ACTIVATE,
	REQ_HOSTREMOVE,
	REQ_MNTCHROOT,
	REQ_GETHOSTS,
	REQ_GETHOST,
	REQ_GETACTIVE,
	REQ_GETSPEC,
	REQ_GETTEMPLATES,
	REQ_STAGE_NEW,
	REQ_TEMPLATE_CREATE,
	REQ_STAGE,
	REQ_GETSTAGED,
	REQ_GETSTAGE,
	REQ_HANDOFF,
	REQ_SAVE,
	REQ_HOSTWRITE,
	
	/* Binary requests */
	REQ_BINSERVER,
	
	/* General */
	REQ_EXIT
} request_t;

typedef enum {
	STRCT_END,
	STRCT_HOST,
} request_structure_t;

struct __HostEdit {
	request_structure_t type;
	char host_id[16];
	size_t selection_one;
	size_t selection_two; //!< -1 for none, >= 0 for vector access at offset_1
};

struct __HTTPRequest {
	char* type;
	char* arg;
	char* version;
	char** headers;
	char* request;
	
	size_t request_size;
};

union __RequestData{
	struct __HostEdit he;
	
};

struct __Request {
	protocol_t protocol;
	request_t request_type;
	void* resolved_call;
	Connection* conn;
	
	RequestData structures;
};

Request* request_handle (Connection* conn);
int http_request_parse (Request* req, HTTPRequest* dest);
response_t request_call (Request* req);


#endif //AUTOGENTOO_REQUEST_H
