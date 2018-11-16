//
// Created by atuser on 2/16/18.
//

#include <autogentoo/http/http.h>
#include <stdlib.h>
#include <string.h>
#include <autogentoo/server.h>
#include <autogentoo/request.h>

HttpRequest* http_request_parse(Request* req) {
	HttpRequest* out = malloc (sizeof (HttpRequest));
	
	printf("%s\n", (char*)req->conn->request);
	char* splt_1 = strpbrk((char*)req->conn->request, "\t ");
	if (!splt_1)
		goto error;
	
	char* splt_2 = strpbrk(splt_1 + 1, "\t\n\r ");
	if (!splt_2)
		goto error;
	
	char* splt_3 = strpbrk(splt_2 + 1, "\t\n\r ");
	if (!splt_3)
		goto error;
	
	//out->function = strndup((char*)req->conn->request, splt_1 - (char*)req->conn->request);
	out->path = strndup(splt_1 + 1, splt_2 - (splt_1 + 1));
	//out->version = strndup (splt_2 + 1, splt_3 - (splt_2 + 1));
	
	char* end_of_body;
	//out->headers = parse_headers(splt_3 + 2, &end_of_body);
	out->request_size = end_of_body - splt_3 + 1;
	out->body = strndup(splt_3 + 1, end_of_body - (splt_3 + 1));
	
	return out;
	
	error:
	free(out);
	return NULL;
}

HttpHeader* parse_headers(char* str, char** end_str) {
	if (*str == 0 || *(str + 1) == 0 || strncmp(str, "\r\n", 2) == 0) {
		*end_str = str;
		return NULL;
	}
	
	HttpHeader* out = malloc (sizeof (HttpHeader));
	char* name_end = strchr (str, ':');
	out->name = strndup (str, name_end - str);
	
	name_end += 2;
	/**     * *   ***/
	/** Size: 2048 **/
	
	char* header_end = strchr(name_end, '\n');
	out->value = strndup (name_end, header_end - name_end);
	
	printf("HEADER: %s\nVALUE:%s\n", out->name, out->value);
	fflush(stdout);
	
	out->next = parse_headers(header_end + 1, end_str); // Plus 1 because of the newline
	
	return out;
}

char* get_header(HttpHeader* HEAD, char* to_find) {
	if (strcmp (HEAD->name, to_find) == 0)
		return HEAD->value;
	
	if (HEAD->next != NULL)
		return get_header(HEAD->next, to_find);
	
	return NULL;
}

void free_headers (HttpHeader* HEAD) {
	free (HEAD->value);
	free (HEAD->name);
	
	if (HEAD->next != NULL)
		free_headers(HEAD->next);
	
	free (HEAD);
}

void http_write_directory (Connection* conn, HttpRequest req) {

}

void free_http_request(HttpRequest* ptr) {
	//free(ptr->function);
	free(ptr->path);
	//free(ptr->version);
	free(ptr->body);
	
	small_map_free(ptr->headers, 1);
}