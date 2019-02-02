//
// Created by atuser on 1/27/19.
//

#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include <autogentoo/api/request_generate.h>
#include "test.h"

void test_htonl(void** state) {
	assert_byte_32(htonl(0x0000aabb), 0, 0, 0xaa, 0xbb);
	assert_byte_32(htonl(0xddccbbaa), 0xdd, 0xcc, 0xbb, 0xaa);
	assert_byte_32(ntohl(0x0000aabb), 0, 0, 0xaa, 0xbb);
	assert_byte_32(ntohl(0xddccbbaa), 0xdd, 0xcc, 0xbb, 0xaa);
	(void) state;
}

void test_htonll(void** state) {
	assert_byte_64(htonll(0xeeffccdd0000aabb), 0xee, 0xff, 0xcc, 0xdd, 0, 0, 0xaa, 0xbb);
	assert_byte_64(htonll(0xaabbccddddccbbaa), 0xaa, 0xbb, 0xcc, 0xdd, 0xdd, 0xcc, 0xbb, 0xaa);
	assert_byte_64(ntohll(0xbbaa00000000aabb), 0xbb, 0xaa, 0, 0, 0, 0, 0xaa, 0xbb);
	assert_byte_64(ntohll(0xeeffccdd0000aabb), 0xee, 0xff, 0xcc, 0xdd, 0, 0, 0xaa, 0xbb);
	(void) state;
}

void test_request(void** state) {
	ClientRequest* req = client_request_init(REQ_INSTALL);
	
	ClientType host_select[] = {"HOST_ID"};
	ClientType authorize[] = {"USER_ID", "TOKEN"};
	ClientType install[] = {"EMERGE_STR"};
	
	client_request_add_structure(req, STRCT_HOSTSELECT, host_select);
	client_request_add_structure(req, STRCT_AUTHORIZE, authorize);
	client_request_add_structure(req, STRCT_HOSTINSTALL, install);
	
	size_t size;
	void* request;
	int size_check = client_request_generate(req, &size, &request);
	assert_int_equal(size, size_check);
	FILE* fp = fopen("client_request_test", "w+");
	fwrite(request, 1, size, fp);
	fclose(fp);
	
	(void) state;
}

void test_dynamic_binary(void** state) {

}

int main(void) {
	const struct CMUnitTest tests[] = {
			cmocka_unit_test(test_htonl),
			cmocka_unit_test(test_htonll),
			cmocka_unit_test(test_request),
	};
	
	return cmocka_run_group_tests(tests, NULL, NULL);
}