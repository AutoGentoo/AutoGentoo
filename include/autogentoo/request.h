//
// Created by atuser on 4/1/18.
//

#ifndef AUTOGENTOO_REQUEST_H
#define AUTOGENTOO_REQUEST_H

typedef struct __Request Request;

typedef enum {
	PROT_AUTOGENTOO = 0, // guarentees first byte is 0 (cant be HTTP)
	PROT_HTTP,
} protocol_t;

typedef enum {
	DIR_CONNECTION_END, //!< Kill the connection after this request ends
	DIR_CONNECTION_OPEN, //!< Handle another request after this one
} directive_t;

typedef enum {
	REQ_GET,
	REQ_INSTALL,
	REQ_EDIT,
	REQ_MNTCHROOT,
	REQ_GETHOSTS,
	REQ_GETHOST,
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
	REQ_EXIT,
	REQ_HEAD,
	REQ_POST,
} request_t;

#include <autogentoo/response.h>
#include <autogentoo/http.h>
#include <autogentoo/request_structure.h>

/**
 * A function pointer for to handle requests
 * @param conn the connection of the request
 * @param args a list of arguments passed to the handler
 * @param the index to start reading the request at
 */
typedef response_t (* HTTP_FH)(Connection* conn, HttpRequest* req);
typedef response_t (* AUTOGENTOO_FH) (Request* request);

typedef union __FunctionHandler FunctionHandler;


union __FunctionHandler {
	AUTOGENTOO_FH ag_fh;
	HTTP_FH http_fh;
};

struct __Request {
	protocol_t protocol;
	request_t request_type;
	FunctionHandler resolved_call;
	Connection* conn;
	directive_t directive;
	
	void* body;
	size_t length;
	
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

/**
 * Links a string to a request handler
 */
struct __RequestLink {
	request_t request_ident; //!< The string that matches the request
	FunctionHandler call; //!< A pointer to the function handler
};

Request* request_handle (Connection* conn);
response_t request_call (Request* req);
void request_free (Request* req);

#endif //AUTOGENTOO_REQUEST_H
