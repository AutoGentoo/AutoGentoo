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
};

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
	creat_tok.access_level = TOKEN_HOST_WRITE;
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
	
	char* directory = server_get_path(request->parent, host->id);
	
	WorkerRequest* strct_worker_request = malloc(sizeof(WorkerRequest));
	strct_worker_request->chroot = 0;
	strct_worker_request->script = "/usr/bin/emerge";
	strct_worker_request->host = host;
	
	StringVector* worker_args = string_vector_new();
	string_vector_add(worker_args, "-v");
	
	char* token = strtok(request->structures[2].emerge.emerge, " ");
	while(token) {
		string_vector_add(worker_args, token);
		token = strtok(NULL, " ");
	}
	string_vector_add(worker_args, NULL);
	
	strct_worker_request->arguments = worker_args->ptr;
	strct_worker_request->argument_n = worker_args->n - 1; // Ignore NULL at end
	
	free(worker_args);
	
	char* job_name = worker_request(request->parent->job_handler, strct_worker_request);
	
	if (!job_name)
		HANDLE_RETURN(INTERNAL_ERROR);
	
	free(directory);
	
	dynamic_binary_add(res->content, 's', job_name);
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

	struct utsname uname_pointer;
	uname(&uname_pointer);
	char* sys_info[][2] = {
			{"System name", uname_pointer.sysname},
			{"Nodename", uname_pointer.nodename},
			{"Release", uname_pointer.release},
			{"Version", uname_pointer.version},
			{"Machine", uname_pointer.machine},
	};

	dynamic_binary_array_start(res->content);
	for (int i = 0; i < sizeof(sys_info) / sizeof(sys_info[0]); i++) {
		dynamic_binary_add(res->content, 's', sys_info[i][0]);
		dynamic_binary_add(res->content, 's', sys_info[i][1]);
		dynamic_binary_array_next(res->content);
	}
	dynamic_binary_array_end(res->content);
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

		dynamic_binary_array_start(res->content);
		for (int j = 0; j < current->make_conf->n; j++) {
			SmallMap_key* variable = *(SmallMap_key**)vector_get(current->make_conf, j);
			dynamic_binary_add(res->content, 's', variable->key);
			dynamic_binary_add(res->content, 's', (char*)variable->data_ptr);
			dynamic_binary_array_next(res->content);
		}
		dynamic_binary_array_end(res->content);
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
	lwarning("Waiting for lock...");
	fflush(stdout);
	pthread_mutex_lock(&request->parent->job_handler->worker_mutex);
	char* job_id = request->structures[2].job_select.job_name;
	Worker* worker;
	for (worker = request->parent->job_handler->worker_head; worker; worker = worker->next)
		if (strcmp(job_id, worker->id) == 0)
			break;
	pthread_mutex_unlock(&request->parent->job_handler->worker_mutex);
	linfo("Finished with lock");
	char* filename;
	asprintf(&filename, "log/%s-%s.log", host->id, job_id);
	
	struct stat log_stat;
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
	
	int c;
	ssize_t log_read_size;
	if (request->conn->communication_type == COM_RSA)
		while ((log_read_size = read(log_fd, &c, sizeof(c))) == sizeof(c))
			SSL_write(request->conn->encrypted_connection, &c, log_read_size);
	else
		HANDLE_RETURN_HTTP(UPGRADE_REQUIRED);
	
	/* Job is done. exit now */
	if (!worker || pthread_mutex_trylock(&worker->running) != EBUSY) {
		linfo("No longer running - %p", worker);
		close(log_fd);
		HANDLE_RETURN_HTTP(OK);
	}
	
	int iwatch = inotify_init();
	if (iwatch < 0) {
		lerror("inotify_init()");
		close(log_fd);
		return;
	}
	
	int watch_d = inotify_add_watch(iwatch, filename, IN_MODIFY | IN_CLOSE_WRITE);
	if (watch_d < 0) {
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
	for (;keep_reading;) {
		read_size = read(iwatch, evbuff, sizeof(struct inotify_event));
		if (read_size <= 0)
			break;
		
		for ( p = evbuff; p < evbuff + read_size; ) {
			event = (struct inotify_event*) p;
			
			if (event->mask & IN_CLOSE_WRITE) {
				if (pthread_mutex_trylock(&worker->running) != EBUSY)
					keep_reading = 0;
			}
			
			if (event->mask & IN_MODIFY) {
				linfo("%s was modified", event->name);
				while ((log_read_size = read(log_fd, &c, sizeof(c))) == sizeof(c)) {
					SSL_write(request->conn->encrypted_connection, &c, log_read_size);
					write(1, &c, 4);
				}
			}
			
			if (pthread_mutex_trylock(&worker->running) != EBUSY)
				keep_reading = 0;
			
			p += sizeof(struct inotify_event) + event->len;
		}
	}
	linfo("log exited");
	
	inotify_rm_watch(iwatch, watch_d);
	close(iwatch);
	close(log_fd);
}
