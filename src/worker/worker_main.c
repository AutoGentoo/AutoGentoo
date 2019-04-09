//
// Created by atuser on 4/6/19.
//

#include "worker.h"
#include <string.h>

#define offsetof(st, m) ((size_t)&(((st *)0)->m))

int main(int argc, char** argv) {
	WorkerHandler* test_handler;
	
	test_handler = worker_handler_new();
	worker_handler_start(test_handler);
	
	return 0;
}