#ifndef __AUTOGENTOO_RESPONSE_H__
#define __AUTOGENTOO_RESPONSE_H__

#include <stdio.h>
#include <autogentoo/api/dynamic_binary.h>

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
	HTTP_UPGRADE_REQUIRED = 426,
	HTTP_INTERNAL_ERROR = 500,
	HTTP_NOT_IMPLEMENTED = 501,
	HTTP_BAD_GATEWAY = 502,
	HTTP_SERVICE_UNAVAILABLE = 503,
	HTTP_CHROOT_NOT_MOUNTED = 504,
	HTTP_CHROOT_INIT = 505,
	SIG_AUTOGENTOO_SEGV = 999
} response_nt;

typedef struct {
	response_nt code;
	char* message;
	size_t len;
} response_t;

typedef struct __Response Response;

struct __Response {
    DynamicBinary* content;
    response_t code;
    int sent_response;
};

#define RESPONSE_SENT (response_t) {0, "", 0}
#define OK (response_t) {HTTP_OK, "Ok", 2}
#define CREATED (response_t) {HTTP_CREATED, "Created", 7}
#define NO_CONTENT (response_t) {HTTP_NO_CONTENT, "No Content", 10}
#define BAD_REQUEST (response_t) {HTTP_BAD_REQUEST, "Bad Request", 11}
#define UNAUTHORIZED (response_t) {HTTP_UNAUTHORIZED, "Unauthorized", 12}
#define FORBIDDEN (response_t) {HTTP_FORBIDDEN, "Forbidden", 9}
#define NOT_FOUND (response_t) {HTTP_NOT_FOUND, "Not found", 9}
#define METHOD_NOT_ALLOWED (response_t) {HTTP_METHOD_NOT_ALLOWED, "Method Not Allowed", 18}
#define REQUEST_TIMEOUT (response_t) {HTTP_REQUEST_TIMEOUT, "Request Timeout", 15}
#define UPGRADE_REQUIRED (response_t) {HTTP_UPGRADE_REQUIRED, "Upgrade Required", 16}
#define INTERNAL_ERROR (response_t) {HTTP_INTERNAL_ERROR, "Internal Error", 14}
#define NOT_IMPLEMENTED (response_t) {HTTP_NOT_IMPLEMENTED, "Method Not Implemented", 22}
#define BAD_GATEWAY (response_t) {HTTP_BAD_GATEWAY, "Bad Gateway", 11}
#define SERVICE_UNAVAILABLE (response_t) {HTTP_SERVICE_UNAVAILABLE, "Service Unavailable", 19}
#define CHROOT_NOT_MOUNTED (response_t) {HTTP_CHROOT_NOT_MOUNTED, "Chroot Not Mounted", 18}
#define CHROOT_INIT (response_t) {HTTP_CHROOT_INIT, "Chroot Initializing", 19}
#define AUTOGENTOO_SEGV (response_t) {SIG_AUTOGENTOO_SEGV, "Segmentation Falt", 17}

extern response_t res_list[];

#define SOCK_CHECK_CONN(sock, ret_size)\
if (fcntl(sock, F_GETFD) == -1 || errno == EBADF) { \
	close (sock); \
	return ret_size; \
}

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

#endif
