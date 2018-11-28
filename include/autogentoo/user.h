//
// Created by atuser on 9/7/18.
//

#ifndef AUTOGENTOO_USER_H
#define AUTOGENTOO_USER_H

#include <stdio.h>
#include <openssl/evp.h>
#include <autogentoo/hacksaw/tools.h>

#define AUTOGENTOO_TOKEN_LENGTH 32
#define AUTOGENTOO_HASH_LENGTH 128

typedef struct __User User;
typedef struct __AuthToken AuthToken;
typedef struct __AccessToken AccessToken;

typedef enum {
	ACCESS_NONE, //!< Can fetch packaged from public hosts
	ACCESS_USER, //!< Can request emerge from authorized hosts, can create new hosts and edit them
	ACCESS_SYSTEM_ADMIN /** Unrestricted access
						 * Only this user may create and delete users
						 */
} access_t;

typedef enum {
	REQUEST_ACCESS_NONE, //!< WHAT ?
	REQUEST_ACCESS_READ, //!< Read only requests
	REQUEST_ACCESS_EMERGE, //!< Read and emerge
	REQUEST_ACCESS_WRITE, //!< Read, emerge, and write requests
	REQUEST_ACCESS_SUPER, //!< All under and create/delete tokens
} token_access_t;

struct __User {
	char* id;
	char* hash;
	char* salt;
	access_t access_level;
	Vector* tokens;
};

struct __AccessToken { // Serverside auth
	char* user_id;
	char* host_id; //!< Target host
	char* auth_token;
	token_access_t access_level;
};

#include "host.h"

#define HOST_AUTHORIZE(___access_level) \
Host* resolved_host = server_get_host(request->conn->parent, request->structures[0].hs.host_id); \
if (!resolved_host) \
	return NOT_FOUND; \
{\
AccessToken hash; \
hash.access_level = (___access_level);\
hash.user_id = request->structures[1].auth.user_id; \
hash.host_id = request->structures[0].hs.host_id; \
hash.auth_token = request->structures[1].auth.token; \
if (host_verify_token(request->conn->parent, &hash) != 0) \
	return FORBIDDEN;} \

int server_create_user(Server* server,
                       char* id,
                       char* hash,
                       char* auth_token,
                       access_t user_level);

User* server_get_user(Server* server, char* user_id);

int host_verify_token(Server* server, AccessToken* request_token);

int user_generate_token(User* user,
                        char* host_id,
                        token_access_t access_level,
                        AccessToken* creation_token);

#endif //AUTOGENTOO_USER_H
