#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <autogentoo/autogentoo.h>
#include <netinet/in.h>
#include <autogentoo/http.h>
#include <autogentoo/user.h>
#include <mcheck.h>
#include <autogentoo/api/dynamic_binary.h>

RequestLink requests[] = {
		{REQ_GET,             {.http_fh=GET}},
		{REQ_HEAD,            {.http_fh=HEAD}},
		{REQ_HOST_NEW,        {.ag_fh=HOST_NEW}},
};

FunctionHandler resolve_call(request_t type) {
	int i;
	for (i = 0; i < sizeof (requests) / sizeof (RequestLink); i++)
		if (requests[i].request_ident == type)
			return requests[i].call;
	FunctionHandler k = {NULL};
	return k;
}

int prv_check_data_structs (Request* req, const int* to_check, int count) {
	if (count != req->struct_c)
		return 1;
	
	for (int i = 0; i < count; i++)
		if (req->types[i] != to_check[i])
			return 1;
	
	return 0;
}

/**
 * PROT_AUTOGENTOO
 */
void HOST_NEW(Response* res, Request* request) {
	HANDLE_CHECK_STRUCTURES({STRCT_AUTHORIZE, STRCT_HOST_NEW});
	AccessToken* tok = authorize (request, TOKEN_SERVER_WRITE, AUTH_TOKEN_HOST);
	
	if (!tok)
		HANDLE_RETURN(FORBIDDEN);
	
	/* Create the host */
	Host* target = host_new(request->parent, host_id_new());
	target->arch = strdup(request->structures[1].host_new.arch);
	target->hostname = strdup(request->structures[1].host_new.hostname);
	target->profile = strdup(request->structures[1].host_new.profile);
	
	AccessToken creat_tok;
	creat_tok.access_level = TOKEN_HOST_WRITE;
	creat_tok.host_id = target->id;
	creat_tok.user_id = request->structures[0].auth.user_id;
	
	AccessToken* issued = auth_issue_token(request->parent, &creat_tok);
	if (!issued)
		HANDLE_RETURN(NOT_FOUND);
	
	dynamic_binary_add(res->content, 's', issued->user_id);
	dynamic_binary_add(res->content, 's', issued->host_id);
	dynamic_binary_add(res->content, 's', issued->auth_token);
	dynamic_binary_add(res->content, 'i', &issued->access_level);
}

void HOST_EDIT(Response* res, Request* request) {
	HANDLE_CHECK_STRUCTURES({STRCT_AUTHORIZE, STRCT_HOST_SELECT, STRCT_HOST_EDIT});
	AccessToken* tok = authorize (request, TOKEN_HOST_WRITE, AUTH_TOKEN_HOST);
	if (!tok)
		HANDLE_RETURN(FORBIDDEN);
	
	HANDLE_GET_HOST(request->structures[1].host_select.hostname);
	struct __struct_Host_edit host_edit = request->structures[2].host_edit;
	
	if (host_edit.request_type == 1) {
		free(small_map_delete(host->make_conf, host_edit.make_conf_var));
		small_map_insert(host->make_conf, host_edit.make_conf_var, strdup(host_edit.make_conf_val));
	}
	else if (host_edit.request_type == 2) {
		if (strcmp(host_edit.make_conf_var, "profile") == 0) {
			if (host->profile)
				free(host->profile);
			host->profile = strdup(host_edit.make_conf_val);
		}
		else if (strcmp(host_edit.make_conf_var, "hostname") == 0) {
			if (host->hostname)
				free(host->hostname);
			host->hostname = strdup(host_edit.make_conf_val);
		}
		else if (strcmp(host_edit.make_conf_var, "arch") == 0) {
			if (host->arch)
				free(host->arch);
			host->arch = strdup(host_edit.make_conf_val);
		}
		else
			HANDLE_RETURN(BAD_REQUEST);
	}
}

void HOST_DEL(Response* res, Request* request) {
	HANDLE_CHECK_STRUCTURES({STRCT_AUTHORIZE, STRCT_HOST_SELECT});
	AccessToken* tok = authorize (request, TOKEN_HOST_MOD, AUTH_TOKEN_HOST);
	if (!tok)
		HANDLE_RETURN(FORBIDDEN);
	
	HANDLE_GET_HOST(request->structures[1].host_select.hostname);
	
	/* Remove the host */
	for (int i = 0; i < request->parent->hosts->n; i++)
		if (*(Host**)vector_get(request->parent->hosts, i) == host) {
			vector_remove(request->parent->hosts, i);
			break;
		}
	
	host_free(host);
}

void HOST_EMERGE(Response* res, Request* request) {
	HANDLE_CHECK_STRUCTURES({STRCT_AUTHORIZE, STRCT_HOST_SELECT, STRCT_EMERGE});
	AccessToken* tok = authorize (request, TOKEN_HOST_MOD, AUTH_TOKEN_HOST);
	if (!tok)
		HANDLE_RETURN(FORBIDDEN);
	
	HANDLE_GET_HOST(request->structures[1].host_select.hostname);
	
	//host_emerge(host, request->structures[2].emerge.emerge);
}

void SRV_MNTCHROOT(Response* res, Request* request);
void SRV_INFO(Response* res, Request* request);
