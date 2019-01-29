//
// Created by atuser on 1/28/19.
//

#ifndef AUTOGENTOO_REQUEST_CHECK_H
#define AUTOGENTOO_REQUEST_CHECK_H

#include "autogentoo/request.h"
#include "request_generate.h"

static struct __request_check {
	request_t type;
	int n;
	request_structure_t args[3];
} request_checks[] = {
		{REQ_INSTALL, 3, {STRCT_HOSTSELECT, STRCT_AUTHORIZE, STRCT_HOSTINSTALL}},
		{REQ_EDIT, 3, {STRCT_HOSTSELECT, STRCT_AUTHORIZE, STRCT_HOSTEDIT}},
		{REQ_MNTCHROOT, 2, {STRCT_HOSTSELECT, STRCT_AUTHORIZE}},
		{REQ_GETHOST, 2, {STRCT_HOSTSELECT, STRCT_AUTHORIZE}},
		{REQ_TEMPLATE_CREATE, 1, {STRCT_TEMPLATECREATE}},
		{REQ_STAGE_NEW, 1, {STRCT_TEMPLATESELECT}},
		{REQ_STAGE, 2, {STRCT_HOSTSELECT, STRCT_STAGECOMMAND}},
		{REQ_GETSTAGE, 1, {STRCT_TEMPLATESELECT}},
		{REQ_HANDOFF, 1, {STRCT_HOSTSELECT}},
		{REQ_HOSTWRITE, 2, {STRCT_HOSTSELECT, STRCT_AUTHORIZE}},
		{REQ_WORKERHANDOFF, 2, {STRCT_HOSTSELECT, STRCT_WORKERRESPONSE}}
};

static struct __request_check request_check_not_found = {-1, 0, {}};

typedef enum {
	REQ_CHK_OK,
	REQ_CHK_EINVALID, /* Request type is invalid */
	REQ_CHK_EINCORRECT_N, /* Incorrect number of arguments */
	REQ_CHK_EINVALID_ARGS /* Invalid argument types */
} request_check_error_t;

request_check_error_t request_check(ClientRequest* req);

#endif //AUTOGENTOO_REQUEST_CHECK_H
