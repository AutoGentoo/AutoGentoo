//
// Created by atuser on 4/7/18.
//

#ifndef AUTOGENTOO_REQUEST_STRUCTURE_H
#define AUTOGENTOO_REQUEST_STRUCTURE_H

typedef union __RequestData RequestData;

typedef enum {
	STRCT_END,
	STRCT_HOSTEDIT,
	STRCT_HOSTSELECT,
	STRCT_HOSTINSTALL,
	STRCT_TEMPLATECREATE,
	STRCT_STAGESELECT,
	STRCT_STAGECOMMAND,
} request_structure_t;

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

typedef enum {
	STAGE_NONE = 0,
	STAGE_DOWNLOAD = 0x1,
	STAGE_EXTRACT = 0x2,
	STAGE_ALL = STAGE_DOWNLOAD | STAGE_EXTRACT
} stage_command_t;

struct __StageCommand {
	 stage_command_t command;
} __attribute__((packed));

static char* request_structure_linkage[] = {
		"iis",
		"s",
		"s",
		"ssssa(si)",
		"i",
		"i"
};

union __RequestData {
	struct __HostEdit he;
	struct __HostSelect hs;
	struct __HostInstall hi;
	struct __TemplateCreate tc;
	struct __StageSelect ss;
	struct __StageCommand sc;
};

int parse_request_structure (RequestData* out, char* template, void* data, void* end);
void free_request_structure (RequestData* to_free, char* template, const char* end);
size_t get_sizeof (char c);
size_t get_ssizeof (char* template);

#endif //AUTOGENTOO_REQUEST_STRUCTURE_H
