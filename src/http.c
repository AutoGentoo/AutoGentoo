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
	return auth_verify_token(parent, &tok) != NULL;
}

void prv_http_free_header(SmallMap* headers) {
	for (int i = 0; i < headers->n; i++) {
		HttpHeader* current_header = small_map_get_index(headers, i);
		free(current_header->name);
		free(current_header->value);
		free(current_header);
	}
	small_map_free(headers, 0);
}

void http_free_request(HttpRequest* ptr) {
	free(ptr->path);
	prv_http_free_header(ptr->headers);
	prv_http_free_header(ptr->response_headers);
	free (ptr);
}

void http_add_header(HttpRequest* req, char* name, char* value) {
	HttpHeader* header = malloc(sizeof(HttpHeader));
	header->name = strdup(name);
	header->value = strdup(value);
	small_map_insert(req->response_headers, header->name, header);
}

void prv_http_get_date(char* dest, size_t len) {
	time_t now = time(0);
	struct tm* time_info = gmtime(&now);
	
	strftime(dest, len, "%a, %d %b %Y %H:%M:%S %Z", time_info);
}

void http_add_default_headers(HttpRequest* req) {
	char date[32];
	prv_http_get_date (date, 32);
	http_add_header (req, "Date", date);
	http_add_header (req, "Server", "AutoGentoo");
}

FILE* http_handle_path(Server* parent, HttpRequest* req, long* size) {
	char* path;
	
	if (*req->path != '/') {
		req->response = BAD_REQUEST;
		return NULL;
	}
	
	char* temp_path = strdup(req->path + 1);
	char* request_type = strtok(temp_path, "/");
	
	if (request_type && strcmp (request_type, "host") == 0) {
		char* host_id = strtok(NULL, "/");
		char* file = strtok(NULL, "\0");
		
		if (http_check_authorization(parent, req, host_id, TOKEN_HOST_READ) != 0) {
			req->response = FORBIDDEN;
			free(temp_path);
			return NULL;
		}
		
		Host* target = server_get_host(parent, host_id);
		char* pkgdir = small_map_get(target->make_conf, "PKGDIR");
		
		asprintf(&path, "%s/%s/%s/%s", parent->location, target->id, pkgdir, file);
	}
	else
		asprintf(&path, "%s/%s", parent->location, req->path);
	
	char path_buffer[PATH_MAX];
	char* resolved_path = realpath(path, path_buffer);
	free(temp_path);
	free(path);
	
	if (!resolved_path) {
		req->response = NOT_FOUND;
		return NULL;
	}
	
	struct stat path_stat;
	stat(resolved_path, &path_stat);
	if (!S_ISREG(path_stat.st_mode)) {
		req->response = NOT_FOUND;
		//free(resolved_path);
		return NULL;
	}
	
	if (strncmp(resolved_path, parent->location, strlen(parent->location)) != 0) {
		req->response = FORBIDDEN;
		//free(resolved_path);
		return NULL;
	}
	
	FILE* fp = fopen(resolved_path, "r");
	if (!fp) {
		req->response = NOT_FOUND;
		return NULL;
	}
	//free(resolved_path);
	
	fseek(fp, 0L, SEEK_END);
	*size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	char* size_buff = malloc((size_t)((int)log10(*size) + 1) + 1);
	sprintf(size_buff, "%ld", *size);
	http_add_header(req, "Content-Length", size_buff);
	free(size_buff);
	
	req->response = OK;
	return fp;
}

size_t http_send_headers(Connection* conn, HttpRequest* req) {
	rsend(conn, req->response);
	
	for (int i = 0; i < req->response_headers->n; i++) {
		HttpHeader* c_header = small_map_get_index(req->response_headers, i);
		conn_write(conn, c_header->name, strlen(c_header->name));
		conn_write(conn, ": ", 2);
		conn_write(conn, c_header->value, strlen(c_header->value));
		conn_write(conn, "\n", 1);
	}
	
	conn_write(conn, "\n", 1);
}

ssize_t http_send_default(Connection* conn, response_t res) {
	rsend(conn, BAD_REQUEST);
	char* default_headers;
	char date[32];
	prv_http_get_date (date, 32);
	
	asprintf(&default_headers, "Date: %s\nServer: AutoGentoo\n\n", date);
	ssize_t out_size = conn_write(conn, default_headers, strlen(default_headers));
	free(default_headers);
	
	return out_size;
}