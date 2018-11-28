//
// Created by atuser on 2/16/18.
//

#include <autogentoo/http.h>
#include <stdlib.h>
#include <string.h>
#include <autogentoo/server.h>
#include <autogentoo/request.h>
#include <autogentoo/user.h>

HttpHeader* http_get_header(HttpRequest* request, char* to_find) {
	return small_map_get(request->headers, to_find);
}

int http_check_authorization(Server* parent, HttpRequest* request, char* host_id, token_access_t access_level) {
	HttpHeader* auth_key = http_get_header(request, "Authorization");
	HttpHeader* user_id = http_get_header(request, "User");
	if (!auth_key || !user_id)
		return -1;
	
	AccessToken tok = {user_id->value, host_id, auth_key->value, access_level};
	return host_verify_token(parent, &tok);
}

void http_free_request(HttpRequest* ptr) {
	free(ptr->path);
	small_map_free(ptr->headers, 1);
	free (ptr);
}