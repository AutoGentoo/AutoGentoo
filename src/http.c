//
// Created by atuser on 2/16/18.
//

#include <autogentoo/http.h>
#include <stdlib.h>
#include <string.h>
#include <autogentoo/server.h>
#include <autogentoo/request.h>

http_request_header* parse_headers (char* str) {
	if (*str == 0 || *str == '\n')
		return NULL;
	
	http_request_header* out = malloc (sizeof (http_request_header));
	char* name_end = strchr (str, ':');
	out->name = strndup (str, name_end - str);
	out->value = strndup (name_end + 2, strchr(name_end, '\n') - name_end - 2);
	out->next = parse_headers(name_end + 3 + strlen (out->value)); // Plus 3 because of the newline
	
	return out;
}

char* get_header(http_request_header* HEAD, char* to_find) {
	if (strcmp (HEAD->name, to_find) == 0) {
		return HEAD->value;
	}
	
	if (HEAD->next != NULL)
		return get_header(HEAD->next, to_find);
	
	return NULL;
}

void free_headers (http_request_header* HEAD) {
	free (HEAD->value);
	free (HEAD->name);
	
	if (HEAD->next != NULL)
		free_headers(HEAD->next);
	
	free (HEAD);
}

void http_write_directory (Connection* conn, HTTPRequest req) {

}