//
// Created by atuser on 1/27/19.
//

#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include <autogentoo/api/request_generate.h>
#include <autogentoo/api/dynamic_binary.h>
#include <autogentoo/api/ssl_wrap.h>
#include <autogentoo/api/ssl_stream.h>
#include "test.h"

void test_htonl(void** state) {
	assert_byte_32(htonl(0x0000aabb), 0, 0, 0xaa, 0xbb)
	assert_byte_32(htonl(0xddccbbaa), 0xdd, 0xcc, 0xbb, 0xaa)
	assert_byte_32(ntohl(0x0000aabb), 0, 0, 0xaa, 0xbb)
	assert_byte_32(ntohl(0xddccbbaa), 0xdd, 0xcc, 0xbb, 0xaa)
	(void) state;
}

void test_htonll(void** state) {
	assert_byte_64(htonll(0xeeffccdd0000aabb), 0xee, 0xff, 0xcc, 0xdd, 0, 0, 0xaa, 0xbb)
	assert_byte_64(htonll(0xaabbccddddccbbaa), 0xaa, 0xbb, 0xcc, 0xdd, 0xdd, 0xcc, 0xbb, 0xaa)
	assert_byte_64(ntohll(0xbbaa00000000aabb), 0xbb, 0xaa, 0, 0, 0, 0, 0xaa, 0xbb)
	assert_byte_64(ntohll(0xeeffccdd0000aabb), 0xee, 0xff, 0xcc, 0xdd, 0, 0, 0xaa, 0xbb)
	(void) state;
}

void test_request(void** state) {
	ClientRequest* req = client_request_init(REQ_HOST_NEW);
	
	DynamicType authorize[] = {"USER_ID", "TOKEN"};
	DynamicType host_new[] = {"ARCH", "PROFILE", "HOSTNAME"};
	
	client_request_add_structure(req, STRCT_AUTHORIZE, authorize);
	client_request_add_structure(req, STRCT_HOST_NEW, host_new);
	
	int size_check = client_request_generate(req);
	assert_int_equal(req->size, size_check);
	FILE* fp = fopen("client_request_test", "w+");
	fwrite(req->ptr, 1, req->size, fp);
	fclose(fp);
	
	client_request_free(req);
	
	(void) state;
}

void test_dynamic_binary(void** state) {
	DynamicType content[] = {
			{.string="hello"},
			{.string="world"},
			{.integer=32},
			{.string="goodbye"},
			{.integer=1},
			{.string="test1"},
			{.integer=2},
			{.string="test2"},
			{.integer=3},
			{.string="test3"},
	};
	
	char* template_full = "ssisaisnisnise";
	
	DynamicBinary* test = dynamic_binary_new(DB_ENDIAN_TARGET_NETWORK);
	dynamic_bin_t out = dynamic_binary_add_quick(test, template_full, content);
	assert_int_equal(out, DYNAMIC_BIN_OK);
	
	free(dynamic_binary_free(test));
	
	(void)state;
}

void test_ssl_client(void** state) {
	autogentoo_client_ssl_init();
	SSocket* ssl_socket;
	assert_int_equal(ssocket_new(&ssl_socket, "localhost", 9491), 0);
	
	ClientRequest* request = client_request_init(REQ_SRV_INFO);
	ssocket_request(ssl_socket, request);
	
	void* res;
	ssize_t res_size = ssocket_read_response(ssl_socket, &res);
	
	FILE* fp = fopen("ssl_test", "w+");
	fwrite(res, 1, (size_t)res_size, fp);
	fclose(fp);
	
	client_request_free(request);
	ssocket_free(ssl_socket);
	free(res);
}

void test_ssl_stream(void** state) {
	SMWServer* server = smw_server_new("9492", "smw.cert", "smw.rsa");
	assert_non_null(server);
}

int main(void) {
	const struct CMUnitTest tests[] = {
			cmocka_unit_test(test_htonl),
			cmocka_unit_test(test_htonll),
			cmocka_unit_test(test_request),
			cmocka_unit_test(test_dynamic_binary),
//			cmocka_unit_test(test_ssl_client),
			cmocka_unit_test(test_ssl_stream),
	};
	
	return cmocka_run_group_tests(tests, NULL, NULL);
}