//
// Created by atuser on 9/7/18.
//

#ifndef AUTOGENTOO_USER_H
#define AUTOGENTOO_USER_H

#include <stdio.h>
#include <openssl/evp.h>
#include <autogentoo/hacksaw/tools.h>

#ifndef AUTOGENTOO_TOKEN_LENGTH
#define AUTOGENTOO_TOKEN_LENGTH 32
#endif

#ifndef AUTOGENTOO_USER_AUTH_ENCRYPT
#error AUTOGENTOO_USER_AUTH_ENCRYPT needs to be generated
#endif

typedef struct __User User;
typedef struct __AuthToken AuthToken;
typedef struct __AccessToken AccessToken;

typedef enum {
	ACCESS_NONE, // Can fetch packaged from public hosts
	ACCESS_USER, // Can request emerge from authorized hosts
	ACCESS_HIGHTENED_USER,  /* Can request to create another host
							 * When host is created, user will move to HOST_ADMIN
							 * This user cannot make changes on HIGHTENED_USER hosts
							 */
	ACCESS_HOST_ADMIN,  /* Can confirm emerge on authorized hosts
						 * Can create new hosts and delete authorized ones */
	ACCESS_SYSTEM_ADMIN /* Unrestricted access
						 * Only this user may create and delete users
						 */
} access_t;

struct __AuthToken {
	char* host_id;
	void* auth_token;
};

struct __User {
	char* id;
	char* hash;
	char* salt;
	Vector* tokens;
};

struct __AccessToken { // Serverside auth
	char* user_id;
	void* auth_token;
	access_t access_level;
};

#include "host.h"

void
host_generate_token(Host* host,
                    AccessToken sysadmin_token,
                    User* target,
                    access_t access_level,
                    AuthToken* out_token);

User* server_create_user(Server* server, char* id, char* hash);

#endif //AUTOGENTOO_USER_H
