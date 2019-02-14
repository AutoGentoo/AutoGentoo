//
// Created by atuser on 9/7/18.
//

#include <autogentoo/user.h>
#include <string.h>
#include <fcntl.h>
#include <autogentoo/request.h>

void prv_random_string(char* out, size_t len) {
	int random_fd = open ("/dev/random", O_RDONLY);
	read(random_fd, out, len);
	close(random_fd);
}

AccessToken* authorize(Request* request, token_access_t access_level, auth_t type) {
	AccessToken req_tok;
	req_tok.user_id = request->structures[0].auth.user_id;
	req_tok.auth_token = request->structures[0].auth.token;
	req_tok.access_level = access_level;
	
	if (type == AUTH_TOKEN_HOST)
		req_tok.host_id = request->structures[1].host_select.hostname;
	else
		req_tok.host_id = NULL;
	
	return auth_verify_token(request->parent, &req_tok);
}

User* server_user_new(Server* server, char* id) {
	if (map_get(server->users, id) != NULL) {
		lderror ("User: %s already exists!", id);
		return NULL;
	}
	
	User* new_user = malloc (sizeof (User));
	
	new_user->id = strdup(id);
	new_user->tokens = vector_new(sizeof(AccessToken*), VECTOR_REMOVE | VECTOR_UNORDERED);
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

User* server_get_user(Server* server, char* user_id) {
	return map_get(server->users, user_id);
}

AccessToken* auth_verify_token(Server* server, AccessToken* request_token) {
	User* user = server_get_user(server, request_token->user_id);
	/* User not found */
	if (!user)
		return NULL;
	
	AccessToken* found_token = prv_user_get_token(user, request_token->auth_token);
	if (!found_token)
		return NULL;
	
	if ((request_token->access_level & found_token->access_level) != found_token->access_level)
		return NULL;
	
	if (found_token->host_id)
		if (strcmp(found_token->host_id, request_token->host_id) != 0)
			return NULL;
	
	return found_token;
}

AccessToken* auth_issue_token(Server* server, AccessToken* creation_token) {
	User* user = server_get_user(server, creation_token->user_id);
	if (!user)
		return NULL;
	
	AccessToken* out = malloc(sizeof(AccessToken));
	out->host_id = strdup(creation_token->host_id);
	out->user_id = strdup(creation_token->user_id);
	out->auth_token = malloc (AUTOGENTOO_TOKEN_LENGTH + 1);
	
	int token_used;
	do {
		token_used = 0;
		 prv_random_string(out->auth_token, AUTOGENTOO_TOKEN_LENGTH);
		for (int i = 0; i < user->tokens->n; i++)
			if (strncmp((*(AccessToken**)vector_get(user->tokens, i))->auth_token,
			            out->auth_token,
			            AUTOGENTOO_TOKEN_LENGTH) == 0) {
				token_used = 1;
				break;
			}
	} while(token_used);
	
	out->auth_token[AUTOGENTOO_TOKEN_LENGTH] = 0;
	vector_add(user->tokens, &out);
	
	return out;
}

/**
 * autogentoo.org registers a user
 * Requires autogentoo_org token */
AccessToken* auth_register_user(Server* server, AccessToken* creation_token, AccessToken* auth_token) {
	if ((auth_token->access_level & server->autogentoo_org_token->access_level) != server->autogentoo_org_token->access_level)
		return NULL;
	
	User* new_user = server_user_new(server, creation_token->user_id);
	if (!new_user) {
		lwarning("User '%s' already exists", creation_token->user_id);
		return NULL;
	}
	
	creation_token->access_level = TOKEN_SERVER_READ;
	return auth_issue_token(server, creation_token);
}

void token_free(AccessToken* tok) {
	free(tok->auth_token);
	free(tok->user_id);
	free(tok->host_id);
	free(tok);
}

void user_free(User* user) {
	free(user->id);
	for (int i = 0; i < user->tokens->n; i++)
		token_free(*(AccessToken**)vector_get(user->tokens, i));
	
	vector_free(user->tokens);
	free(user);
}