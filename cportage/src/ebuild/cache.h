//
// Created by atuser on 12/31/19.
//

#ifndef AUTOGENTOO_CACHE_H
#define AUTOGENTOO_CACHE_H

typedef struct __CacheHandler CacheHandler;
typedef struct __CacheWorker CacheWorker;

struct __CacheWorker {
	CacheHandler* handler;
	
	pthread_t pid;
	int index;
};

struct __CacheHandler {
	struct __CacheRequest {
		Ebuild* target;
		struct __CacheRequest* next;
	}* head;
	
	pthread_mutex_t head_mutex;
	pthread_cond_t cond;
	
	Vector* pids;
	
	int number;
	int jobs;
};

#include "../constants.h"

CacheWorker* cache_worker_new(CacheHandler* parent);
CacheHandler* cache_handler_new(int jobs);
void cache_handler_request(CacheHandler* handler, Ebuild* target);
void cache_request_mainloop(CacheWorker* worker);
void cache_handler_finish(CacheHandler* handler);
int cache_verify(char* file, char* md5_expanded);

int cache_generate(Ebuild* target, int thread_num);

void ebuild_metadata(Ebuild* ebuild);

#endif //AUTOGENTOO_CACHE_H
