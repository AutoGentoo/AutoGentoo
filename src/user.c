//
// Created by atuser on 9/7/18.
//

#include <autogentoo/user.h>
#include <string.h>
#include <fcntl.h>

void prv_random_string(char* out, size_t len) {
	int random_fd = open ("/dev/random", O_RDONLY);
	read(random_fd, out, len);
	close(random_fd);
}

int server_create_user(Server* server,
                       char* id,
                       char* hash,
                       char* auth_token,
                       access_t user_level) {
	if (map_get(server->users, id) != NULL) {
		lderror ("User: %s already exists!", id);
		return 1;
	}
	if (!auth_token && user_level > ACCESS_USER) {
		lderror("auth token is required to create sys_admin");
		return 2;
	}
	
	User* new_user = malloc (sizeof (User));
	
	new_user->id = strdup(id);
	new_user->hash = strdup(id);
	//new_user->salt = strdup(AUTOGENTOO_USER_AUTH_ENCRYPT);
	
	new_user->tokens = vector_new(sizeof(AccessToken*), REMOVE | UNORDERED);
	map_insert(server->users, new_user->id, new_user);
	
	return 0;
}

AccessToken* prv_user_get_token(User* user, char* auth_token) {
	for (int i = 0; i < user->tokens->n; i++) {
		AccessToken* current_tok = *(AccessToken**)vector_get(user->tokens, i);
		if (strncmp(current_tok->auth_token, auth_token, AUTOGENTOO_TOKEN_LENGTH) == 0)
			return current_tok;
	}
	
	return NULL;
}

#define HOST_VERIFY_STMT(stmt, ...) \
if (stmt) { \
	lderror(__VA_ARGS__); \
	return error_no; \
} \
error_no++;

int host_verify_token(Server* server, AccessToken* request_token) {
	int error_no = 1;
	if (!request_token->user_id) {
		Host* target = server_get_host(server, request_token->host_id);
		HOST_VERIFY_STMT(request_token->access_level != REQUEST_ACCESS_NONE, "requested access too high");
		HOST_VERIFY_STMT(target->private, "host is not public");
	}
	
	
	User* target_user = map_get(server->users, request_token->user_id);
	HOST_VERIFY_STMT(!target_user, "user '%s' not found", request_token->user_id);
	HOST_VERIFY_STMT(strlen(request_token->auth_token) != AUTOGENTOO_TOKEN_LENGTH, "len(token) error");
	
	AccessToken* resolved_token = prv_user_get_token(target_user, request_token->auth_token);
	HOST_VERIFY_STMT (resolved_token == NULL, "invalid token");
	HOST_VERIFY_STMT(strncmp(resolved_token->host_id,
					request_token->host_id,
					AUTOGETNOO_HOST_ID_LENGTH) == 0, "host_id incorrect");
	HOST_VERIFY_STMT(request_token->access_level > resolved_token->access_level, "insufficient permissions");
	
	return 0;
}

char* prv_gen_random(size_t len);

int user_generate_token(User* user,
                        char* host_id,
                        token_access_t access_level,
                        AccessToken* creation_token) {
	int token_used = 0;
	
	if (strncmp(creation_token->host_id, host_id, AUTOGETNOO_HOST_ID_LENGTH) != 0) {
		lderror("creation_token has incorrect host_id");
		return 1;
	}
	
	if (creation_token->access_level != REQUEST_ACCESS_SUPER) {
		lderror("creation_token access level not high enough");
		return 2;
	}
	
	do {
		token_used = 0;
		creation_token->auth_token = prv_gen_random(AUTOGENTOO_TOKEN_LENGTH);
		for (int i = 0; i < user->tokens->n; i++)
			if (strncmp((*(AccessToken**)vector_get(user->tokens, i))->auth_token,
					creation_token->auth_token,
					AUTOGENTOO_TOKEN_LENGTH) == 0) {
				token_used = 1;
				free(creation_token->auth_token);
				break;
			}
	} while(token_used);
	
	vector_add(user->tokens, &creation_token);
	return 0;
}