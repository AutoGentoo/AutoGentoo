#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <autogentoo/handle.h>
#include <autogentoo/http.h>
#include <autogentoo/user.h>
#include <autogentoo/api/dynamic_binary.h>
#include <autogentoo/chroot.h>
#include <sys/utsname.h>
#include <autogentoo/writeconfig.h>
#include <errno.h>
#include <sys/inotify.h>
#include "autogentoo/worker.h"
#include <openssl/ssl.h>

RequestLink requests[] = {
		{REQ_GET,             {.http_fh=GET}},
		{REQ_HEAD,            {.http_fh=HEAD}},
		{REQ_HOST_NEW,        {.ag_fh=HOST_NEW}},
		{REQ_HOST_EDIT,       {.ag_fh=HOST_EDIT}},
		{REQ_HOST_DEL,        {.ag_fh=HOST_DEL}},
		{REQ_HOST_EMERGE,     {.ag_fh=HOST_EMERGE}},
		{REQ_HOST_MNTCHROOT,  {.ag_fh=HOST_MNTCHROOT}},
		{REQ_AUTH_ISSUE_TOK,  {.ag_fh=AUTH_ISSUE_TOK}},
		{REQ_AUTH_REFRESH_TOK,{.ag_fh=AUTH_REFRESH_TOK}},
		{REQ_SRV_INFO,        {.ag_fh=SRV_INFO}},
		{REQ_SRV_REFRESH,     {.ag_fh=SRV_REFRESH}},
		{REQ_AUTH_REGISTER,   {.ag_fh=AUTH_REGISTER}},
		{REQ_JOB_STREAM,      {.ag_fh=JOB_STREAM}},
		{REQ_META_STAGE3,     {.ag_fh=META_STAGE3}},
};

RequestNameLink request_names[] = {
		{REQ_GET,             "GET"},
		{REQ_HEAD,            "HEAD"},
		{REQ_HOST_NEW,        "HOST_NEW"},
		{REQ_HOST_EDIT,       "HOST_EDIT"},
		{REQ_HOST_DEL,        "HOST_DEL"},
		{REQ_HOST_EMERGE,     "HOST_EMERGE"},
		{REQ_HOST_MNTCHROOT,  "HOST_MNTCHROOT"},
		{REQ_AUTH_ISSUE_TOK,  "AUTH_ISSUE_TOK"},
		{REQ_AUTH_REFRESH_TOK,"AUTH_REFRESH_TOK"},
		{REQ_SRV_INFO,        "SRV_INFO"},
		{REQ_SRV_REFRESH,     "SRV_REFRESH"},
		{REQ_AUTH_REGISTER,   "AUTH_REGISTER"},
		{REQ_JOB_STREAM,      "JOB_STREAM"},
		{REQ_META_STAGE3,     "META_STAGE3"}
};

char* str_request(request_t type) {
	int i;
	for (i = 0; i < sizeof (request_names) / sizeof (RequestNameLink); i++)
		if (request_names[i].ident == type)
			return request_names[i].name;
	return NULL;
}

FunctionHandler resolve_call(request_t type) {
	int i;
	for (i = 0; i < sizeof (requests) / sizeof (RequestLink); i++)
		if (requests[i].request_ident == type)
			return requests[i].call;
	FunctionHandler ret;
	ret.ag_fh = NULL;
	return ret;
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
	AccessToken* tok = authorize (request, TOKEN_SERVER_WRITE, AUTH_TOKEN_SERVER);
	
	if (!tok)
		HANDLE_RETURN(FORBIDDEN);

	/* Create the host */
	Host* target = host_new(request->parent, host_id_new());
	target->arch = strdup(request->structures[1].host_new.arch);
	target->hostname = strdup(request->structures[1].host_new.hostname);
	target->profile = strdup(request->structures[1].host_new.profile);

	AccessToken creat_tok;
	creat_tok.access_level = TOKEN_HOST_MOD;
	creat_tok.host_id = target->id;
	creat_tok.user_id = request->structures[0].auth.user_id;

	AccessToken* issued = auth_issue_token(request->parent, &creat_tok);
	if (!issued)
		HANDLE_RETURN(NOT_FOUND);

	vector_add(request->parent->hosts, &target);

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

	HANDLE_GET_HOST(request->structures[1].host_select.hostname)
	struct __struct_Host_edit host_edit = request->structures[2].host_edit;
	
	
	HANDLE_RETURN(INTERNAL_ERROR);
}

void HOST_DEL(Response* res, Request* request) {
	HANDLE_CHECK_STRUCTURES({STRCT_AUTHORIZE, STRCT_HOST_SELECT});
	AccessToken* tok = authorize (request, TOKEN_HOST_MOD, AUTH_TOKEN_HOST);
	if (!tok)
		HANDLE_RETURN(FORBIDDEN);

	HANDLE_GET_HOST(request->structures[1].host_select.hostname)

	/* Remove the host */
	for (int i = 0; i < request->parent->hosts->n; i++)
		if (*(Host**)vector_get(request->parent->hosts, i) == host) {
			vector_remove(request->parent->hosts, i);
			break;
		}

	host_free(host);
	write_server(request->parent);
}

void HOST_EMERGE(Response* res, Request* request) {
	HANDLE_CHECK_STRUCTURES({STRCT_AUTHORIZE, STRCT_HOST_SELECT, STRCT_EMERGE});
	AccessToken* tok = authorize (request, TOKEN_HOST_EMERGE, AUTH_TOKEN_HOST);
	if (!tok)
		HANDLE_RETURN(FORBIDDEN);
	HANDLE_GET_HOST(request->structures[1].host_select.hostname)
	
	
	WorkerRequest* strct_worker_request = malloc(sizeof(WorkerRequest));
	strct_worker_request->command_name = "emerge";
	
	DynamicBinary* db = dynamic_binary_new(DB_ENDIAN_INPUT_NETWORK);
	
	dynamic_binary_add(db, 's', request->structures[1].host_select.hostname);
	dynamic_binary_array_start(db);
	
	char* token = strtok(request->structures[2].emerge.emerge, " ");
	while(token) {
		dynamic_binary_add(db, 's', token);
		token = strtok(NULL, " ");
	}
	dynamic_binary_array_end(db);
	
	strct_worker_request->bytes = db->ptr;
	strct_worker_request->n = db->used_size;
	strct_worker_request->template = db->template;
	
	char* job_name;
	int worker_res = worker_handler_request(request->parent->job_handler, strct_worker_request, &job_name);
	
	if (!job_name)
		HANDLE_RETURN(INTERNAL_ERROR);
	
	dynamic_binary_add(res->content, 's', job_name);
	
	dynamic_binary_free(db);
	free(strct_worker_request);
	
	HANDLE_RETURN(get_res(worker_res));
}

void HOST_MNTCHROOT(Response* res, Request* request) {
	HANDLE_CHECK_STRUCTURES({STRCT_AUTHORIZE, STRCT_HOST_SELECT});
	AccessToken* tok = authorize (request, TOKEN_HOST_MOD, AUTH_TOKEN_HOST);
	if (!tok)
		HANDLE_RETURN(FORBIDDEN);

	HANDLE_GET_HOST(request->structures[1].host_select.hostname)

	response_t ret = chroot_mount(host);
	HANDLE_RETURN(ret);
}

void SRV_INFO(Response* res, Request* request) {
	HANDLE_CHECK_STRUCTURES({});

	char* distro = "Unknown";
	
	FILE* os_fp = fopen("/etc/os-release", "r");
	if (os_fp) {
		ssize_t nread = 0;
		size_t len = 0;
		char* line = NULL;
		
		while ((nread = getline(&line, &len, os_fp)) != -1)
			if (strncmp(line, "NAME=", 5) == 0) {
				distro = strndup(line + 5, nread - 6);
				break;
			}
		
		if (line)
			free(line);
	}
	
	struct utsname uname_pointer;
	uname(&uname_pointer);
	char* sys_info[][2] = {
			{"OS Name", uname_pointer.sysname},
			{"Hostname", uname_pointer.nodename},
			{"Kernel", uname_pointer.release},
			{"Arch", uname_pointer.machine},
			{"Distro", distro}
	};

	dynamic_binary_array_start(res->content);
	for (int i = 0; i < sizeof(sys_info) / sizeof(sys_info[0]); i++) {
		dynamic_binary_add(res->content, 's', sys_info[i][0]);
		dynamic_binary_add(res->content, 's', sys_info[i][1]);
		dynamic_binary_array_next(res->content);
	}
	dynamic_binary_array_end(res->content);
	
	free(distro);
}

void SRV_REFRESH(Response* res, Request* request) {
	HANDLE_CHECK_STRUCTURES({STRCT_AUTHORIZE});
	AccessToken* tok = authorize (request, TOKEN_SERVER_AUTOGENTOO_ORG, AUTH_TOKEN_SERVER);
	if (!tok)
		HANDLE_RETURN(FORBIDDEN);

	dynamic_binary_array_start(res->content);
	for (int i = 0; i < request->parent->hosts->n; i++) {
		Host* current = *(Host**)vector_get(request->parent->hosts, i);
		dynamic_binary_add(res->content, 's', current->id);
		dynamic_binary_add(res->content, 's', current->hostname);
		dynamic_binary_add(res->content, 's', current->profile);
		dynamic_binary_add(res->content, 's', current->arch);
		dynamic_binary_add(res->content, 'i', &current->environment_status);

		/*dynamic_binary_array_start(res->content);
		for (int j = 0; j < current->make_conf->n; j++) {
			SmallMap_key* variable = *(SmallMap_key**)vector_get(current->make_conf, j);
			dynamic_binary_add(res->content, 's', variable->key);
			dynamic_binary_add(res->content, 's', (char*)variable->data_ptr);
			dynamic_binary_array_next(res->content);
		}
		dynamic_binary_array_end(res->content);*/
		dynamic_binary_array_next(res->content);
	}
	dynamic_binary_array_end(res->content);
}

void AUTH_ISSUE_TOK(Response* res, Request* request) {
	HANDLE_CHECK_STRUCTURES({STRCT_AUTHORIZE, STRCT_HOST_SELECT, STRCT_ISSUE_TOK});
	AccessToken auth_tok;
	AccessToken creat_tok;

	auth_tok.host_id = request->structures[1].host_select.hostname;
	auth_tok.auth_token = request->structures[0].auth.token;
	auth_tok.user_id = request->structures[0].auth.user_id;

	creat_tok.user_id = request->structures[2].issue_tok.user_id;
	creat_tok.host_id = request->structures[2].issue_tok.target_host;
	creat_tok.access_level = request->structures[2].issue_tok.permission;

	AccessToken* issued = autogentoo_issue(request->parent, &auth_tok, &creat_tok);
	if (!issued)
		HANDLE_RETURN(FORBIDDEN);

	dynamic_binary_add(res->content, 's', issued->auth_token);
	dynamic_binary_add(res->content, 's', issued->user_id);
	dynamic_binary_add(res->content, 's', issued->host_id);
	dynamic_binary_add(res->content, 'i', &issued->access_level);

}

void AUTH_REFRESH_TOK(Response* res, Request* request) {
	HANDLE_CHECK_STRUCTURES({STRCT_AUTHORIZE});

	AccessToken* tok = authorize (request, TOKEN_SERVER_AUTOGENTOO_ORG, AUTH_TOKEN_SERVER);
	//if (!tok)
	//	HANDLE_RETURN(FORBIDDEN);

	StringVector* token_keys = map_all_keys(request->parent->auth_tokens);

	dynamic_binary_array_start(res->content);
	for (int i = 0; i < token_keys->n; i++) {
		AccessToken* token = map_get(request->parent->auth_tokens, string_vector_get(token_keys, i));
		if (!token) {
			string_vector_free(token_keys);
			HANDLE_RETURN(INTERNAL_ERROR);
		}

		dynamic_binary_add(res->content, 's', token->auth_token);
		dynamic_binary_add(res->content, 's', token->user_id);
		dynamic_binary_add(res->content, 's', token->host_id);
		dynamic_binary_add(res->content, 'i', &token->access_level);
		dynamic_binary_array_next(res->content);
	}
	dynamic_binary_array_end(res->content);
	string_vector_free(token_keys);
}

void AUTH_REGISTER(Response* res, Request* request) {
	HANDLE_CHECK_STRUCTURES({STRCT_AUTHORIZE, STRCT_ISSUE_TOK});
	AccessToken auth_tok;
	AccessToken creat_tok;

	AccessToken* tok = authorize (request, TOKEN_SERVER_AUTOGENTOO_ORG, AUTH_TOKEN_SERVER);
	if (!tok)
		HANDLE_RETURN(FORBIDDEN);

	creat_tok.user_id = request->structures[1].issue_tok.user_id;
	creat_tok.host_id = NULL;
	creat_tok.access_level = TOKEN_SERVER_WRITE;
	AccessToken* issued = auth_issue_token(request->parent, &creat_tok);
	if (!issued)
		HANDLE_RETURN(INTERNAL_ERROR);

	dynamic_binary_add(res->content, 's', issued->auth_token);
	dynamic_binary_add(res->content, 's', issued->user_id);
	dynamic_binary_add(res->content, 's', issued->host_id);
	dynamic_binary_add(res->content, 'i', &issued->access_level);
}

void JOB_STREAM(Response* res, Request* request) {
	HANDLE_CHECK_STRUCTURES({STRCT_AUTHORIZE, STRCT_HOST_SELECT, STRCT_JOB_SELECT});
	AccessToken* tok = authorize (request, TOKEN_HOST_READ, AUTH_TOKEN_HOST);
	if (!tok)
		HANDLE_RETURN_HTTP(FORBIDDEN);
	Host* host = server_get_host(request->parent, request->structures[1].host_select.hostname); \
	if (!host) { \
		token_free(map_remove(request->parent->auth_tokens, request->structures[0].auth.token)); \
		HANDLE_RETURN_HTTP(NOT_FOUND);\
	}
	
	/* Check if worker is running */
	char* job_id = request->structures[2].job_select.job_name;
	
	char* filename;
	asprintf(&filename, "log/%s-%s.log", host->id, job_id);
	size_t filename_len = strlen(filename);
	
	char* lock_file = malloc(filename_len + 5);
	sprintf(lock_file, "%s.lck", filename);
	
	int running = 0;
	
	struct stat log_stat;
	if (stat(lock_file, &log_stat) == 0) /* Successful lock file stat() */
		running = 1;
	
	if (stat(filename, &log_stat) != 0) {
		lerror("Failed to open %s", filename);
		lerror("Error [%d] %s", errno, strerror(errno));
		
		if (errno == ENOENT)
			HANDLE_RETURN_HTTP(NOT_FOUND);
		HANDLE_RETURN_HTTP(INTERNAL_ERROR);
	}
	
	int log_fd = open(filename, O_RDONLY);
	if (log_fd == -1) {
		lerror ("Failed to open %s for reading", filename);
		lerror ("Error [%d] %s", errno, strerror(errno));
		HANDLE_RETURN_HTTP(INTERNAL_ERROR);
	}
	
	START_STREAM()
	
	/* Dump the existing content of the file to SSL */
	int c;
	ssize_t log_read_size;
	if (request->conn->communication_type == COM_RSA) {
		while ((log_read_size = read(log_fd, &c, sizeof(c))) == sizeof(c))
			if (SSL_write(request->conn->encrypted_connection, &c, log_read_size) <= 0) {
				// Client closed connection
				HANDLE_RETURN(OK);
				break;
			}
	}
	else
		HANDLE_RETURN_HTTP(UPGRADE_REQUIRED);
	
	/* Job is done. exit now */
	if (!running) {
		close(log_fd);
		HANDLE_RETURN_HTTP(OK);
	}
	
	int iwatch = inotify_init();
	if (iwatch < 0) {
		lerror("inotify_init()");
		close(log_fd);
		return;
	}
	
	int watch_d = inotify_add_watch(iwatch, filename, IN_MODIFY);
	int watch_d2 = inotify_add_watch(iwatch, lock_file, IN_DELETE_SELF);
	if (watch_d < 0 || watch_d2 < 0) {
		lerror("inotify_add_watch()");
		close(log_fd);
		close(iwatch);
		HANDLE_RETURN_HTTP(INTERNAL_ERROR);
	}
	
	size_t evlen = (10 * (sizeof(struct inotify_event) + NAME_MAX + 1));
	char evbuff[evlen] __attribute__ ((aligned(8)));
	char* p;
	struct inotify_event* event;
	
	res->code = OK;
	ssize_t read_size;
	
	int keep_reading = 1;
	while (keep_reading) {
		read_size = read(iwatch, evbuff, sizeof(struct inotify_event));
		if (read_size <= 0)
			break;
		
		for ( p = evbuff; p < evbuff + read_size; ) {
			event = (struct inotify_event*) p;
			
			if (event->mask & IN_DELETE_SELF)
				keep_reading = 0;
			
			if (event->mask & IN_MODIFY) {
				while ((log_read_size = read(log_fd, &c, sizeof(c))) == sizeof(c)) {
					if (SSL_write(request->conn->encrypted_connection, &c, log_read_size) <= 0) {
						keep_reading = 0;
						break;
					}
				}
			}
			
			p += sizeof(struct inotify_event) + event->len;
		}
	}
	
	inotify_rm_watch(iwatch, watch_d);
	close(iwatch);
	close(log_fd);
}

void META_STAGE3(Response* res, Request* request) {
	HANDLE_CHECK_STRUCTURES({STRCT_AUTHORIZE, STRCT_HOST_SELECT, STRCT_JOB_SELECT});
	
}