//
// Created by atuser on 9/7/18.
//

#ifndef AUTOGENTOO_USER_H
#define AUTOGENTOO_USER_H

#include <stdio.h>
#include <openssl/evp.h>
#include <autogentoo/hacksaw/tools.h>
#include <autogentoo/request_structure.h>

#define AUTOGENTOO_TOKEN_LENGTH 32
#define AUTOGENTOO_HASH_LENGTH 128

typedef struct __User User;
typedef struct __AuthToken AuthToken;
typedef struct __AccessToken AccessToken;

typedef enum {
	TOKEN_HOST_NONE, //!< For private hosts
	TOKEN_HOST_READ = 1,
	TOKEN_HOST_EMERGE = TOKEN_HOST_READ | 1 << 1, //!< Can't change host settings
	TOKEN_HOST_WRITE = TOKEN_HOST_EMERGE | 1 << 2,
	TOKEN_SERVER_READ = 1 << 3,
	TOKEN_SERVER_WRITE = TOKEN_SERVER_READ | 1 << 4, //!< Create hosts
	TOKEN_SERVER_SUPER = 0xFF, //!< All permissions
	TOKEN_SERVER_AUTOGENTOO_ORG = 1 << 5 //!< Register users from server (no read/write)
} token_access_t;

typedef enum {
	AUTH_TOKEN_NONE,
	AUTH_TOKEN_SERVER,
	AUTH_TOKEN_HOST
} auth_t;

struct __User {
	char* id;
	Vector* tokens;
};

struct __AccessToken { // Serverside auth
	char* user_id;
	char* host_id; //!< Target host
	char* auth_token;
	token_access_t access_level;
};

#include "host.h"
#include <autogentoo/request.h>

AccessToken* authorize(Request* request, token_access_t access_level, auth_t type);

User* server_get_user(Server* server, char* user_id);

AccessToken* auth_verify_token(Server* server, AccessToken* request_token);

AccessToken* auth_issue_token(Server* server, AccessToken* creation_token);

/**
 * autogentoo.org registers a user
 * Requires autogentoo_org token */
AccessToken* auth_register_user(Server* server, AccessToken* creation_token, AccessToken* auth_token);

void token_free(AccessToken* tok);
void user_free(User* user);

#endif //AUTOGENTOO_USER_H
