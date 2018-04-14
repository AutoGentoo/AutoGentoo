//
// Created by atuser on 4/7/18.
//

#ifndef AUTOGENTOO_REQUEST_STRUCTURE_H
#define AUTOGENTOO_REQUEST_STRUCTURE_H

#include <autogentoo/request.h>

typedef union __RequestData RequestData;

typedef enum {
	STRCT_END,
	STRCT_HOSTEDIT,
	STRCT_HOSTSELECT,
} request_structure_t;

struct __HostEdit {
	size_t selection_one;
	size_t selection_two; //!< -1 for none, >= 0 for vector access at offset_1
	
	char* edit;
} __attribute__((packed));

struct __HostSelect {
	char* host_id;
} __attribute__((packed));

static char* request_structure_linkage[] = {
		"iis",
		"s"
};

union __RequestData {
	struct __HostEdit he;
	struct __HostSelect hs;
};

int parse_request_structure(RequestData* out, request_structure_t type, void* data, void* end);
void free_request_structure(RequestData* to_free, request_structure_t type);

#endif //AUTOGENTOO_REQUEST_STRUCTURE_H
