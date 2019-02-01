//
// Created by atuser on 1/28/19.
//

#include <autogentoo/api/request_check.h>

request_check_error_t request_check(ClientRequest* req) {
	if (req->request_type >= REQ_MAX)
		return REQ_CHK_EINVALID;
	
	struct __request_check found = request_check_not_found;
	for (int i = 0; i < sizeof(request_checks) / sizeof(request_checks[0]); i++)
		if (request_checks[i].type == req->request_type)
			found = request_checks[i];
	
	if (req->arguments && req->arguments->n != found.n)
		return REQ_CHK_EINCORRECT_N;
	
	if (req->types && req->types->n != found.n)
		return REQ_CHK_EINCORRECT_N;
	
	for (int i = 0; i < found.n; i++)
		if (found.args[i] != *(request_structure_t*)vector_get(req->types, i))
			return REQ_CHK_EINVALID_ARGS;
	
	return REQ_CHK_OK;
}