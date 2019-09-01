//
// Created by atuser on 4/7/18.
//

#ifndef __AUTOGENTOO_REQUEST_STRUCTURE_H
#define __AUTOGENTOO_REQUEST_STRUCTURE_H

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
	STRCT_HOST_META, /* Get information about a potential host */
	
	STRCT_MAX
} request_structure_t;

typedef union __RequestData RequestData;


struct __struct_Host_new {
	char* arch;
	char* profile;
	char* hostname;
};

struct __struct_Host_edit {
	int request_type;
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
	int permission;
} __attribute__((packed));

struct __struct_Host_meta {
	char* arch;
	
	int systemd;

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

char* request_structure_template_parse(char* c_template, size_t* skip);
size_t request_structure_get_size(char* template);
void* request_structure_read(void* src, char* template, size_t* skip);
void request_structure_free(void* rs, char* template);

#endif //AUTOGENTOO_REQUEST_STRUCTURE_H
