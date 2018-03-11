#ifndef __AUTOGENTOO_RESPONSE_H__
#define __AUTOGENTOO_RESPONSE_H__

#include <stdio.h>

typedef enum {
	HTTP_OK = 200,
	HTTP_CREATED = 201,
	HTTP_NO_CONTENT = 204,
	HTTP_BAD_REQUEST = 400,
	HTTP_UNAUTHORIZED = 401,
	HTTP_FORBIDDEN = 403,
	HTTP_NOT_FOUND = 404,
	HTTP_METHOD_NOT_ALLOWED = 405,
	HTTP_REQUEST_TIMEOUT = 408,
	HTTP_INTERNAL_ERROR = 500,
	HTTP_NOT_IMPLEMENTED = 501,
	HTTP_BAD_GATEWAY = 502,
	HTTP_SERVICE_UNAVAILABLE = 503
} response_nt;

typedef struct {
	response_nt code;
	char* message;
	size_t len;
} response_t;


#define NONE (response_t) {0, "", 0}
#define OK (response_t) {HTTP_OK, "OK", 2}
#define CREATED (response_t) {HTTP_CREATED, "Created", 7}
#define NO_CONTENT (response_t) {HTTP_NO_CONTENT, "No Content", 10}
#define BAD_REQUEST (response_t) {HTTP_BAD_REQUEST, "Bad Request", 11}
#define UNAUTHORIZED (response_t) {HTTP_UNAUTHORIZED, "Unauthorized", 12}
#define FORBIDDEN (response_t) {HTTP_FORBIDDEN, "Forbidden", 9}
#define NOT_FOUND (response_t) {HTTP_NOT_FOUND, "Not found", 9}
#define METHOD_NOT_ALLOWED (response_t) {HTTP_METHOD_NOT_ALLOWED, "Method Not Allowed", 18}
#define REQUEST_TIMEOUT (response_t) {HTTP_REQUEST_TIMEOUT, "Requeset Timeout", 16}
#define INTERNAL_ERROR (response_t) {HTTP_INTERNAL_ERROR, "Internal Error", 14}
#define NOT_IMPLEMENTED (response_t) {HTTP_NOT_IMPLEMENTED, "Method Not Implemented", 22}
#define BAD_GATEWAY (response_t) {HTTP_BAD_GATEWAY, "Bad Gateway", 11}
#define SERVICE_UNAVAILABLE (response_t) {HTTP_SERVICE_UNAVAILABLE, "Service Unavailable", 19}

extern response_t res_list[];

#include <sys/types.h>
#include <sys/socket.h>
#include "server.h"

/**
 * Send a HTTP response back to the client
 * @param conn the connection to respond to
 * @param code the code to write
 * @return the number of bytes that were written
 */
ssize_t rsend(Connection* conn, response_t code);

/**
 * Get corresponding response_t given an HTTP response int
 * @param x the int (HTTP res)
 * @return the corresponding response_t
 */
response_t get_res(response_nt x);

ssize_t conn_write (int fd, void* data, size_t len);

#endif
