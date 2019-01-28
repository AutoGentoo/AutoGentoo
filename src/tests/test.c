//
// Created by atuser on 1/27/19.
//

#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
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

int main(void) {
	const struct CMUnitTest tests[] = {
			cmocka_unit_test(test_htonl),
			cmocka_unit_test(test_htonll),
	};
	
	return cmocka_run_group_tests(tests, NULL, NULL);
}