//
// Created by atuser on 4/1/18.
//

#ifndef AUTOGENTOO_REQUEST_H
#define AUTOGENTOO_REQUEST_H

#include <autogentoo/api/dynamic_binary.h>

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
	REQ_HEAD,
	REQ_POST,
	REQ_START,
	
	REQ_HOST_NEW,
	REQ_HOST_EDIT,
	REQ_HOST_DEL,
	REQ_HOST_EMERGE,
	
	REQ_HOST_MNTCHROOT,
	REQ_SRV_INFO,
	REQ_AUTH_ISSUE_TOK,
	REQ_AUTH_REFRESH_TOK,
	
	REQ_MAX
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
typedef void (* HTTP_FH)(Connection* conn, HttpRequest* req);
typedef struct __Response Response;
typedef void (* AUTOGENTOO_FH) (Response* res, Request* request);

typedef union __FunctionHandler FunctionHandler;

union __FunctionHandler {
	AUTOGENTOO_FH ag_fh;
	HTTP_FH http_fh;
};

struct __Response {
	DynamicBinary* content;
	response_t code;
};

struct __Request {
	Server* parent;
	Connection* conn;
	protocol_t protocol;
	request_t request_type;
	FunctionHandler resolved_call;
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
void request_call(Response* res, Request* req);
void request_free (Request* req);

#endif //AUTOGENTOO_REQUEST_H
