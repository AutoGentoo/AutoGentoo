//
// Created by atuser on 2/16/18.
//

#define _GNU_SOURCE

#include <autogentoo/http.h>
#include <stdlib.h>
#include <string.h>
#include <autogentoo/server.h>
#include <autogentoo/request.h>
#include <autogentoo/user.h>
#include <math.h>

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

void prv_http_free_header(SmallMap* headers) {
	for (int i = 0; i < headers->n; i++) {
		HttpHeader* current_header = small_map_get_index(headers, i);
		free(current_header->name);
		free(current_header->value);
		free(small_map_get_key_index(headers, i));
	}
	small_map_free(headers, 1);
}

void http_free_request(HttpRequest* ptr) {
	free(ptr->path);
	prv_http_free_header(ptr->headers);
	prv_http_free_header(ptr->response_headers);
	free (ptr);
}

void http_add_header(HttpRequest* req, char* name, char* value) {
	HttpHeader header = {strdup(name), strdup(value)};
	small_map_insert(req->response_headers, name, &header);
}

void prv_http_get_date(char** dest, size_t len) {
	time_t now = time(0);
	struct tm tm = *gmtime(&now);
	strftime(*dest, len, "%a, %d %b %Y %H:%M:%S %Z", &tm);
}

void http_add_default_headers(HttpRequest* req) {
	char date[128];
	prv_http_get_date ((char**)&date, 128);
	
	http_add_header (req, "Date", date);
	http_add_header (req, "Server", "AutoGentoo");
}

FILE* http_handle_path(Server* parent, HttpRequest* req, long* size) {
	char req_type[32];
	char host_id[16];
	
	size_t path_length = strlen(req->path);
	char req_to_end[path_length];
	char* path;
	
	if (sscanf(req->path, "/%[^/]/%15s/%s", req_type, host_id, req_to_end) == 3 && strcmp(req_type, "host") == 0) {
		if (http_check_authorization(parent, req, host_id, REQUEST_ACCESS_READ) != 0) {
			req->response = FORBIDDEN;
			return NULL;
		}
		
		Host* target = server_get_host(parent, host_id);
		asprintf(&path, "%s/%s/%s/%s", parent->location, target->id, target->pkgdir, req_to_end);
	}
	else
		path = strdup(req->path);
	
	char* resolved_path = realpath(path, NULL);
	free(path);
	
	if (strncmp(resolved_path, parent->location, strlen(parent->location)) != 0) {
		req->response = FORBIDDEN;
		return NULL;
	}
	
	
	FILE* fp = fopen(resolved_path, "r");
	if (!fp) {
		req->response = NOT_FOUND;
		return NULL;
	}
	
	fseek(fp, 0L, SEEK_END);
	*size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	char* size_buff = malloc(sizeof(char)*((int)log10(*size) + 1));
	sprintf(size_buff, "%ld", *size);
	http_add_header(req, "Content-Length", size_buff);
	free(size_buff);
	
	req->response = OK;
	return fp;
}

ssize_t __prv_conn_write(Connection* conn, void* data, size_t len);

size_t http_send_headers(Connection* conn, HttpRequest* req) {
	rsend(conn, req->response);
	
	for (int i = 0; i < req->response_headers->n; i++) {
		HttpHeader* c_header = small_map_get_index(req->response_headers, i);
		__prv_conn_write(conn, c_header->name, strlen(c_header->name));
		__prv_conn_write(conn, ": ", 2);
		__prv_conn_write(conn, c_header->value, strlen(c_header->value));
		__prv_conn_write(conn, "\n", 1);
	}
	
	__prv_conn_write(conn, "\n", 1);
}

ssize_t http_send_bad_request(Connection* conn) {
	rsend(conn, BAD_REQUEST);
	char* default_headers;
	char date[128];
	prv_http_get_date ((char**)&date, 128);
	
	asprintf(&default_headers, "Date: %s\nServer: AutoGentoo\n\n", date);
	ssize_t out_size = __prv_conn_write(conn, default_headers, strlen(default_headers));
	free(default_headers);
	
	return out_size;
}