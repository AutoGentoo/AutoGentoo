//
// Created by atuser on 4/1/18.
//

#ifndef AUTOGENTOO_REQUEST_H
#define AUTOGENTOO_REQUEST_H

#include "response.h"
#include "request_structure.h"

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

typedef union __FunctionHandler FunctionHandler;

typedef enum {
	PROT_AUTOGENTOO = 0, // guarentees first byte is 0 (cant be HTTP)
	PROT_HTTP,
	PROT_AUTOGENTOO_S // Arbitrary number, switch on request byte
} protocol_t;

typedef enum {
	REQ_SWITCH_SECURE = -1, // Switch protocol to PROT_AUTOGENTOO_S
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
	REQ_HOSTUPLOAD,
	
	/* Binary requests */
	REQ_BINSERVER,
	REQ_BINQUEUE,
	
	REQ_WORKERHANDOFF,
	REQ_WORKERMAKECONF,
	
	/* General */
	REQ_EXIT
} request_t;

struct __HTTPRequest {
	char* function;
	char* arg;
	char* version;
	char** headers;
	char* body;
	
	size_t request_size;
};

union __FunctionHandler {
	AUTOGENTOO_FH ag_fh;
	HTTP_FH http_fh;
};

struct __Request {
	protocol_t protocol;
	request_t request_type;
	FunctionHandler resolved_call;
	Connection* conn;
	
	int struct_c;
	Vector* structures_parent;
	Vector* types_parent;
	
	RequestData* structures;
	request_structure_t* types;
};

/**
 * Links a string to a request handler
 */
typedef struct __RequestLink RequestLink;

#include "request.h"

/**
 * Links a string to a request handler
 */
struct __RequestLink {
	request_t request_ident; //!< The string that matches the request
	FunctionHandler call; //!< A pointer to the function handler
	
};

Request* request_handle (Connection* conn);
int http_request_parse (Request* req, HTTPRequest* dest);
response_t request_call (Request* req);
void request_free (Request* req);


#endif //AUTOGENTOO_REQUEST_H
