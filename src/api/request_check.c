//
// Created by atuser on 1/28/19.
//

#include <autogentoo/api/request_check.h>

request_check_error_t request_check(ClientRequest* req) {
	if (req->request_type >= REQ_MAX)
		return REQ_CHK_EINVALID;
	
	struct __request_check found;
	for (int i = 0; i < sizeof(request_checks) / sizeof(request_checks[0]); i++)
		if (request_checks[i].type == req->request_type)
			found = request_checks[i];
	
	if (req->arguments && req->arguments->n != )
	
	return REQ_CHK_OK;
}