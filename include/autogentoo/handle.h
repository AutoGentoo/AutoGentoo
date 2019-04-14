#ifndef __AUTOGENTOO_HANDLE_H__
#define __AUTOGENTOO_HANDLE_H__

#include "request.h"
#include "endian_convert.h"

#define HANDLE_RETURN(ret) ({ \
res->code = ((ret)); \
return; \
})

#define HTTP_RESPONSE_SEND() ({\
	rsend(request->conn, res->code); \
	res->sent_response = 1; \
})

#define HANDLE_RETURN_HTTP(ret) ({\
	res->code = ((ret)); \
	HTTP_RESPONSE_SEND(); \
	return; \
})

#define HANDLE_GET_HOST(name) \
Host* host = server_get_host(request->parent, request->structures[1].host_select.hostname); \
if (!host) { \
	token_free(map_remove(request->parent->auth_tokens, request->structures[0].auth.token)); \
	HANDLE_RETURN(NOT_FOUND);\
}

#define START_STREAM() \
if (!res->sent_response) {\
	HTTP_RESPONSE_SEND(); \
}

#define HANDLE_CHECK_STRUCTURES(...) (\
{ \
	int check_types[] = __VA_ARGS__; \
	int __check_types_n = sizeof (check_types) / sizeof (int); \
	if (__check_types_n != request->struct_c) \
		HANDLE_RETURN(BAD_REQUEST); \
	if (prv_check_data_structs (request, check_types, __check_types_n) == -1) \
		HANDLE_RETURN(BAD_REQUEST); \
})

/**
 * Holds all of the valid requests
 */
extern RequestLink requests[];

/**
 * Parse the request and write the arguemnts to args
 * @param type resolved type from request_handle
 * @return a pointer to function that should be called
 */
FunctionHandler resolve_call(request_t type);


/**
 * HTTP Requests
 * Just normal http
 * We don't do POST around here
 */
void GET(Connection* conn, HttpRequest* req);
void HEAD(Connection* conn, HttpRequest* req);

/**
 * PROT_AUTOGENTOO
 */
void HOST_NEW(Response* res, Request* request);
void HOST_EDIT(Response* res, Request* request);
void HOST_DEL(Response* res, Request* request);
void HOST_EMERGE(Response* res, Request* request);
void HOST_MNTCHROOT(Response* res, Request* request);

void SRV_INFO(Response* res, Request* request);
void SRV_REFRESH(Response* res, Request* request);
void AUTH_ISSUE_TOK(Response* res, Request* request);
void AUTH_REFRESH_TOK(Response* res, Request* request);
void AUTH_REGISTER(Response* res, Request* request);
void JOB_STREAM(Response* res, Request* request);

#endif