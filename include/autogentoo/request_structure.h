//
// Created by atuser on 4/7/18.
//

#ifndef AUTOGENTOO_REQUEST_STRUCTURE_H
#define AUTOGENTOO_REQUEST_STRUCTURE_H

#include <stdio.h>

typedef enum {
	STRCT_END,
	STRCT_HOST_NEW = 1,
	STRCT_HOST_SELECT,
	STRCT_HOST_EDIT,
	STRCT_AUTHORIZE,
	STRCT_EMERGE,
	STRCT_ISSUE_TOK,
	STRCT_JOB_SELECT,
	
	STRCT_MAX
} request_structure_t;

typedef union __RequestData RequestData;

#include "user.h"

struct __struct_Host_new {
	char* arch;
	char* profile;
	char* hostname;
};

struct __struct_Host_edit {
	int request_type; /* 1: make.conf, 2: general */
	char* make_conf_var; /* Must be listed in allowed vars */
	char* make_conf_val;
};

struct __struct_Host_select {
	char* hostname;
};

struct __struct_Authorize {
	char* user_id;
	char* token;
} __attribute__((packed));

struct __struct_Emerge {
	char* emerge;
} __attribute__((packed));

struct __struct_Job {
	char* job_name;
} __attribute__((packed));

struct __struct_Issue_token {
	char* user_id;
	char* target_host;
	token_access_t permission;
} __attribute__((packed));

struct __Raw {
	size_t n;
	void* data;
};

static char* request_structure_linkage[] = {
	"sss", /* Host new */
	"s", /* Host select */
	"iss", /* Host edit */
	"ss", /* Host authorize */
	"s", /* Emerge arguments */
	"ssi", /* Issue Token */
	"s"  /* Job select */
};

union __RequestData {
	struct __struct_Host_new host_new;
	struct __struct_Host_select host_select;
	struct __struct_Host_edit host_edit;
	struct __struct_Authorize auth;
	struct __struct_Issue_token issue_tok;
	struct __struct_Emerge emerge;
	struct __struct_Job job_select;
};

int parse_request_structure(RequestData* out, char* template, void* data, void* end_ptr);
void free_request_structure (RequestData* to_free, char* template, void* end_ptr);
size_t get_item_size(char* template);

#endif //AUTOGENTOO_REQUEST_STRUCTURE_H
