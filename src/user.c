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

void host_generate_token(
		Host* host,
		AccessToken sysadmin_token,
		User* target,
		access_t access_level,
		AuthToken* out_token) {
	out_token->host_id = strdup(target->id);
	//out_token->auth_token = malloc (AUTOGENTOO_TOKEN_LENGTH);
	
	AccessToken host_access_token;
	//host_access_token.auth_token = malloc(AUTOGENTOO_TOKEN_LENGTH);
	//memcpy(host_access_token.auth_token, out_token->auth_token, AUTOGENTOO_TOKEN_LENGTH);
	//host_access_token.access_level = access_level;
	//host_access_token.user_id = strdup(target->id);
	
	//vector_add(host->auth_tokens, &host_access_token);
}

User* server_create_user (Server* server, char* id, char* hash) {
	User* new_user = malloc (sizeof (User));
	
	new_user->id = strdup(id);
	new_user->hash = strdup(id);
	//new_user->salt = strdup(AUTOGENTOO_USER_AUTH_ENCRYPT);
	
	new_user->tokens = vector_new(sizeof(AccessToken), REMOVE | UNORDERED);
}