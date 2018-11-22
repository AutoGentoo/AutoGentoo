//
// Created by atuser on 2/16/18.
//

#include <autogentoo/http.h>
#include <stdlib.h>
#include <string.h>
#include <autogentoo/server.h>
#include <autogentoo/request.h>

HttpHeader* get_header(HttpRequest* request, char* to_find) {
	return small_map_get(request->headers, to_find);
}

void free_http_request(HttpRequest* ptr) {
	free(ptr->path);
	small_map_free(ptr->headers, 1);
	free (ptr);
}