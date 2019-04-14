//
// Created by atuser on 2/3/19.
//

#include <autogentoo/server.h>
#include <autogentoo/http.h>
#include <openssl/ssl.h>

void GET(Connection* conn, HttpRequest* req) {
	/** HTTP/1.0 or HTTP/1.1 **/
	if (req->version.maj != 1 && (req->version.min != 1 || req->version.min != 0))
		req->response = BAD_REQUEST;
	
	long file_size;
	FILE* fp = http_handle_path(conn->parent, req, &file_size);
	
	http_send_headers(conn, req);
	
	if (req->response.code != HTTP_OK) {
		if (fp)
			fclose(fp);
		return;
	}
	
	/* Send the file */
	ssize_t read_len;
	ssize_t total_write = 0;
	int chunk;
	while ((read_len = fread(&chunk, 1, sizeof(chunk), fp)) != 0)
		total_write += connection_write(conn, &chunk, (size_t)read_len);
	req->response = OK;
	
	fclose(fp);
}

void HEAD(Connection* conn, HttpRequest* req) {
	/** HTTP/1.0 or HTTP/1.1 **/
	if (req->version.maj != 1 && (req->version.min != 1 || req->version.min != 0))
		req->response = BAD_REQUEST;
	
	long file_size;
	FILE* fp = http_handle_path(conn->parent, req, &file_size);
	http_send_headers(conn, req);
	
	if (fp)
		fclose(fp);
}

