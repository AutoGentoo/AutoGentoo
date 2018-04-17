//
// Created by atuser on 4/7/18.
//

#ifndef AUTOGENTOO_REQUEST_STRUCTURE_H
#define AUTOGENTOO_REQUEST_STRUCTURE_H

#include <stdio.h>

typedef union __RequestData RequestData;

struct __HostEdit {
	int selection_one;
	int selection_two; //!< -1 for none, >= 0 for vector access at offset_1
	
	char* edit;
} __attribute__((packed));


struct __TemplateCreate {
	char* id;
	char* arch;
	char* cflags;
	char* chost;
	int make_extra_c;
	struct {
		char* make_extra;
		int select;
	} *extras __attribute__((packed));
} __attribute__((packed));

struct __HostSelect {
	char* host_id;
} __attribute__((packed));

struct __StageSelect {
	int index;
} __attribute__((packed));

struct __HostInstall {
	char* argument;
} __attribute__((packed));

struct __HostOffset {
	size_t offset_index;
	size_t n;
	void* data;
} __attribute__((packed));

typedef enum {
	STAGE_NONE = 0,
	STAGE_DOWNLOAD = 0x1,
	STAGE_EXTRACT = 0x2,
	STAGE_ALL = STAGE_DOWNLOAD | STAGE_EXTRACT
} stage_command_t;

struct __StageCommand {
	 stage_command_t command;
} __attribute__((packed));

typedef enum {
	STRCT_HOSTEDIT = 1,
	STRCT_HOSTSELECT,
	STRCT_HOSTINSTALL,
	STRCT_TEMPLATECREATE,
	STRCT_TEMPLATESELECT,
	STRCT_STAGECOMMAND,
	STRCT_HOSTOFFSET, /* Custom offset on Host struct */
} request_structure_t;

static char* request_structure_linkage[] = {
		"iis",
		"s",
		"s",
		"ssssa(si)",
		"i",
		"i",
		"iv",
};

union __RequestData {
	struct __HostEdit he;
	struct __HostSelect hs;
	struct __HostInstall hi;
	struct __TemplateCreate tc;
	struct __StageSelect ss;
	struct __StageCommand sc;
	struct __HostOffset ho;
};

int parse_request_structure (RequestData* out, char* template, void* data, void* end);
void free_request_structure (RequestData* to_free, char* template, const char* end);
size_t get_sizeof (char c);
size_t get_ssizeof (char* template);

#endif //AUTOGENTOO_REQUEST_STRUCTURE_H
