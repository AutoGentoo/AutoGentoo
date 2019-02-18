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

typedef struct __AuthToken AuthToken;
typedef struct __AccessToken AccessToken;

typedef enum {
	TOKEN_NONE,
	TOKEN_SERVER_READ = 1 << 0,
	TOKEN_SERVER_WRITE = TOKEN_SERVER_READ | 1 << 1, //!< Create hosts
	TOKEN_SERVER_AUTOGENTOO_ORG = 1 << 2, //!< Register users from server (no read/write)
	TOKEN_HOST_READ = 1 << 3,
	TOKEN_HOST_EMERGE = TOKEN_HOST_READ | 1 << 4, //!< Can't change host settings
	TOKEN_HOST_WRITE = TOKEN_HOST_EMERGE | 1 << 5, //!< Write to make.conf
	TOKEN_HOST_MOD = TOKEN_HOST_WRITE | 1 << 6, //!< Can delete host
	TOKEN_SERVER_SUPER = 0xFF, //!< All permissions
} token_access_t;

typedef enum {
	AUTH_TOKEN_NONE,
	AUTH_TOKEN_SERVER,
	AUTH_TOKEN_HOST
} auth_t;

struct __AccessToken { // Serverside auth
	char* user_id;
	char* host_id; //!< Target host
	char* auth_token;
	token_access_t access_level;
};

#include "host.h"
#include <autogentoo/request.h>

AccessToken* authorize(Request* request, token_access_t access_level, auth_t type);

AccessToken* auth_verify_token(Server* server, AccessToken* request_token);

AccessToken* autogentoo_issue(Server* server, AccessToken* auth_tok, AccessToken* creat_tok);

AccessToken* auth_issue_token(Server* server, AccessToken* creation_token);

void token_free(AccessToken* tok);

#endif //AUTOGENTOO_USER_H
