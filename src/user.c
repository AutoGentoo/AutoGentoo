//
// Created by atuser on 9/7/18.
//

#include <autogentoo/user.h>
#include <string.h>
#include <fcntl.h>
#include <autogentoo/request.h>

pthread_mutex_t random_mutex;

int prv_random_string(char* out, size_t len) {
	pthread_mutex_lock(&random_mutex);
	
	int rand_d = open("/dev/urandom", O_RDONLY);
	
	char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	size_t chars_len = 62;
	
	int c;
	for (int i = 0; i < len; i++) {
		read(rand_d, &c, sizeof(int));
		out[i] = chars[c % chars_len];
	}
	close(rand_d);
	out[len] = 0;
	
	pthread_mutex_unlock(&random_mutex);
	
	return 0;
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

AccessToken* autogentoo_issue(Server* server, AccessToken* auth_tok, AccessToken* creat_tok) {
	auth_tok->access_level = creat_tok->access_level;
	
	AccessToken* resolved = auth_verify_token(server, auth_tok);
	if (!resolved)
		return NULL;
	
	if (creat_tok->host_id && *creat_tok->host_id && resolved->host_id) {
		if (strcmp(resolved->host_id, creat_tok->host_id) != 0)
			return NULL;
	}
	else if (creat_tok->host_id != resolved->host_id) // NULL
		return NULL;
	
	return auth_issue_token(server, creat_tok);
}

AccessToken* auth_verify_token(Server* server, AccessToken* request_token) {
	AccessToken* found_token = map_get(server->auth_tokens, request_token->auth_token);
	if (!found_token)
		return NULL;
		
	if ((request_token->access_level & found_token->access_level) != request_token->access_level)
		return NULL;
	
	if (found_token->host_id && *found_token->host_id)
		if (strcmp(found_token->host_id, request_token->host_id) != 0)
			return NULL;
	
	return found_token;
}

AccessToken* auth_issue_token(Server* server, AccessToken* creation_token) {
	AccessToken* out = malloc(sizeof(AccessToken));
	if (creation_token->host_id)
		out->host_id = strdup(creation_token->host_id);
	else
		out->host_id = NULL;
	out->user_id = strdup(creation_token->user_id);
	out->auth_token = malloc (AUTOGENTOO_TOKEN_LENGTH + 1);
	out->access_level = creation_token->access_level;
	printf("%d\n", out->access_level);
	
	do
		if (prv_random_string(out->auth_token, AUTOGENTOO_TOKEN_LENGTH) != 0) {
			token_free(out);
			return NULL;
		}
	while(map_get(server->auth_tokens, out->auth_token) != NULL);
	
	map_insert(server->auth_tokens, out->auth_token, out);
	return out;
}

void token_free(AccessToken* tok) {
	free(tok->auth_token);
	free(tok->user_id);
	free(tok->host_id);
	free(tok);
}

void init_random_mutex() {
	pthread_mutex_init(&random_mutex, NULL);
}

void free_random_mutex() {
	pthread_mutex_destroy(&random_mutex);
}