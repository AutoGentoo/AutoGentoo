#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <autogentoo/autogentoo.h>
#include <netinet/in.h>

RequestLink requests[] = {
		{REQ_GET,             {.http_fh=GET}},
		{REQ_INSTALL,         {.ag_fh=INSTALL}},
		{REQ_EDIT,            {.ag_fh=SRV_EDIT}},
		{REQ_ACTIVATE,        {.ag_fh=SRV_ACTIVATE}},
		{REQ_HOSTREMOVE,      {.ag_fh=SRV_HOSTREMOVE}},
		{REQ_MNTCHROOT,       {.ag_fh=SRV_MNTCHROOT}},
		{REQ_GETHOSTS,        {.ag_fh=SRV_GETHOSTS}},
		{REQ_GETHOST,         {.ag_fh=SRV_GETHOST}},
		{REQ_GETACTIVE,       {.ag_fh=SRV_GETACTIVE}},
		{REQ_GETSPEC,         {.ag_fh=SRV_GETSPEC}},
		{REQ_GETTEMPLATES,    {.ag_fh=SRV_GETTEMPLATES}},
		{REQ_STAGE_NEW,       {.ag_fh=SRV_STAGE_NEW}},
		{REQ_TEMPLATE_CREATE, {.ag_fh=SRV_TEMPLATE_CREATE}}, // Alias for the previous one
		{REQ_STAGE,           {.ag_fh=SRV_STAGE}},
		{REQ_GETSTAGED,       {.ag_fh=SRV_GETSTAGED}},
		{REQ_GETSTAGE,        {.ag_fh=SRV_GETSTAGE}},
		{REQ_HANDOFF,         {.ag_fh=SRV_HANDOFF}},
		{REQ_SAVE,            {.ag_fh=SRV_SAVE}},
		{REQ_HOSTWRITE,       {.ag_fh=SRV_HOSTWRITE}},
		{REQ_HOSTUPLOAD,      {.ag_fh=SRV_HOSTUPLOAD}},
		
		/* Binary requests */
		{REQ_BINSERVER,       {.ag_fh=BIN_SERVER}},
		{REQ_BINQUEUE,        {.ag_fh=BIN_QUEUE}},
		
		/* Worker requests */
		{REQ_WORKERHANDOFF,   {.ag_fh=WORKER_HANDOFF}},
		{REQ_WORKERMAKECONF,  {.ag_fh=WORKER_MAKECONF}},
		
		/* General */
		{REQ_EXIT,            {.ag_fh=EXIT}}
};

static struct {
	char* ident;
	request_t type;
} http_links[] = {
		{"GET", REQ_GET}
};

FunctionHandler http_resolve_call (char* function) {
	request_t t = (request_t)-1;
	
	int i;
	for (i = 0; i < sizeof (http_links) / sizeof (http_links[0]); i++) {
		if (strcmp (function, http_links[i].ident) == 0) {
			t = http_links[i].type;
			break;
		}
	}
	
	FunctionHandler k = {NULL};
	if (t == -1)
		return k;
	return resolve_call(t);
}

FunctionHandler resolve_call (request_t type) {
	int i;
	for (i = 0; i < sizeof (requests) / sizeof (RequestLink); i++)
		if (requests[i].request_ident == type)
			return requests[i].call;
	FunctionHandler k = {NULL};
	return k;
}

response_t GET (Connection* conn, HTTPRequest req) {
	response_t res;
	
	if (conn->bounded_host == NULL)
		return FORBIDDEN;
	
	if (!(strncmp(req.version, "HTTP/1.0", 8) == 0 || strncmp(req.version, "HTTP/1.1", 8) == 0))
		return BAD_REQUEST;
	
	char* path;
	asprintf(&path, "%s/%s/%s/%s", conn->parent->location, conn->bounded_host->id, conn->bounded_host->pkgdir, req.arg);
	
	int fd, data_to_send;
	
	if ((fd = open(path, O_RDONLY)) != -1) // FILE FOUND
	{
		ssize_t bytes_read;
		rsend(conn, OK);
		res = OK;
		conn_write(conn->fd, "\n", 1);
		while ((bytes_read = read(fd, (void*) &data_to_send, sizeof(data_to_send))) > 0)
			conn_write(conn->fd, (void*) &data_to_send, (size_t) bytes_read);
		close(fd);
	} else {
		rsend(conn, NOT_FOUND);
		res = NOT_FOUND;
	}
	
	free(path);
	res.len = 0;
	return res;
}

int prv_check_data_structs (Request* req, const int* to_check, int count) {
	if (count != req->struct_c)
		return 1;
	
	for (int i = 0; i < count; i++)
		if (req->types[i] != to_check[i])
			return 1;
	
	return 0;
}

int prv_pipe_to_client (int conn_fd, int* backup_conn) {
	int out = dup(1);
	*backup_conn = conn_fd;
	dup2(conn_fd, 1);
	
	return out;
}

void prv_pipe_back (int* conn_fd, int backup, int backup_conn) {
	close(1);
	dup2(backup, 1);
	*conn_fd = backup_conn;
}

char prv_conn_read_str (char** dest, char* request, int* offset, size_t size) {
	if (*offset >= size)
		return 1;
	
	*dest = request + *offset;
	*offset += strlen(*dest) + 1;
	return 0;
}

char prv_conn_read_int (int* dest, char* request, int* offset, size_t size) {
	if (*offset >= size)
		return 1;
	
	memcpy(dest, request + *offset, sizeof(int));
	*offset += sizeof(int);
	*dest = ntohl((uint32_t) *dest);
	return 0;
}

response_t INSTALL (Request* request) {
	CHECK_STRUCTURES({STRCT_HOSTINSTALL});
	
	if (request->conn->bounded_host == NULL)
		return FORBIDDEN;
	
	if (request->conn->bounded_host->chroot_status == CHR_NOT_MOUNTED)
		return CHROOT_NOT_MOUNTED;
	
	int backup_conn, backup_stdout;
	backup_stdout = prv_pipe_to_client(request->conn->fd, &backup_conn);
	response_t res = host_install(request->conn->bounded_host, request->structures[0].hi.argument);
	prv_pipe_back(&request->conn->fd, backup_stdout, backup_conn);
	
	return res;
}

response_t SRV_CREATE (Connection* conn, char** args, int start) {
	return METHOD_NOT_ALLOWED;
}

response_t SRV_EDIT (Request* request) {
	CHECK_STRUCTURES({STRCT_HOSTSELECT, STRCT_HOSTEDIT});
	
	Host* target = server_host_search(request->conn->parent, request->structures[0].hs.host_id);
	if (!target)
		return NOT_FOUND;
	
	int field_one = request->structures[1].he.selection_one;
	int field_two = request->structures[1].he.selection_two;
	
	if (field_one == 4) {
		if (field_two >= target->extra->n)
			string_vector_add(target->extra, request->structures[1].he.edit);
		else {
			void** t_ptr = vector_get(target->extra, field_two);
			free(*t_ptr);
			
			if (strlen(request->structures[1].he.edit) == 0)
				vector_remove(target->extra, field_two);
			else
				*t_ptr = strdup(request->structures[1].he.edit);
		}
	} else {
		if (field_one == 0) {
			free(target->hostname);
			target->hostname = strdup(request->structures[1].he.edit);
		} else if (field_one == 1) {
			free(target->profile);
			target->profile = strdup(request->structures[1].he.edit);
		} else if (field_one == 2) {
			free(target->cflags);
			target->cflags = strdup(request->structures[1].he.edit);
		} else if (field_one == 3) {
			free(target->use);
			target->use = strdup(request->structures[1].he.edit);
		} else
			return BAD_REQUEST;
	}
	
	return OK;
}

response_t SRV_ACTIVATE (Request* request) {
	CHECK_STRUCTURES({STRCT_HOSTSELECT})
	Host* found = server_host_search(request->conn->parent, request->structures[0].hs.host_id);
	
	if (!found)
		return NOT_FOUND;
	
	server_bind(request->conn, found);
	
	return OK;
}

response_t SRV_HOSTREMOVE (Request* request) {
	CHECK_STRUCTURES({STRCT_HOSTSELECT});
	
	int i;
	// Remove the binding
	for (i = 0; i != request->conn->parent->host_bindings->n; i++) {
		Host** tmp = (Host**) (((void***) vector_get(request->conn->parent->host_bindings, i))[1]);
		if (strcmp((*tmp)->id, request->structures[0].hs.host_id) == 0) {
			vector_remove(request->conn->parent->host_bindings, i);
			// dont break because multiple clients can point to the same host
		}
	}
	
	// Remove the definition
	for (i = 0; i != request->conn->parent->hosts->n; i++) {
		if (strcmp((*(Host**) vector_get(request->conn->parent->host_bindings, i))->id, request->structures[0].hs.host_id) == 0) {
			vector_remove(request->conn->parent->host_bindings, i);
			break; // Two hosts cant have the same id (at least they are not support to...)
		}
	}
	
	return OK;
}

/* SRV Utility request */

response_t SRV_MNTCHROOT (Request* request) {
	if (request->conn->bounded_host == NULL)
		return FORBIDDEN;
	
	return chroot_mount(request->conn->bounded_host);
}

void prv_fd_write_str (int fd, char* str) {
	if (str == NULL) {
		return;
	}
	conn_write(fd, str, strlen(str));
	conn_write(fd, "\n", 1);
}

/* SRV Metadata requests */
response_t SRV_GETHOST (Request* request) {
	CHECK_STRUCTURES ({STRCT_HOSTSELECT});
	
	Host* host = server_host_search(request->conn->parent, request->structures[0].hs.host_id);
	
	if (host == NULL)
		return NOT_FOUND;
	
	if (host->extra != NULL) {
		char t[8];
		sprintf(t, "%d", (int) host->extra->n);
		prv_fd_write_str(request->conn->fd, t);
		
	}
	prv_fd_write_str(request->conn->fd, host->cflags);
	prv_fd_write_str(request->conn->fd, host->cxxflags);
	prv_fd_write_str(request->conn->fd, host->chost);
	prv_fd_write_str(request->conn->fd, host->use);
	prv_fd_write_str(request->conn->fd, host->hostname);
	prv_fd_write_str(request->conn->fd, host->profile);
	
	if (host->extra != NULL)
		for (int i = 0; i != host->extra->n; i++) {
			char* current_str = string_vector_get(host->extra, i);
			conn_write(request->conn->fd, current_str, strlen(current_str));
			conn_write(request->conn->fd, "\n", 1);
		}
	
	return OK;
}

response_t SRV_GETHOSTS (Request* request) {
	char t[8];
	sprintf(t, "%d\n", (int) request->conn->parent->hosts->n);
	conn_write(request->conn->fd, t, strlen(t));
	
	int i;
	for (i = 0; i != request->conn->parent->hosts->n; i++) {
		char* temp = (*(Host**) vector_get(request->conn->parent->hosts, i))->id;
		conn_write(request->conn->fd, temp, strlen(temp));
		conn_write(request->conn->fd, "\n", 1);
	}
	
	return OK;
}

response_t SRV_GETACTIVE (Request* request) {
	if (request->conn->bounded_host == NULL) {
		char* out = "invalid\n";
		conn_write(request->conn->fd, out, strlen(out));
		return NOT_FOUND;
	}
	
	conn_write(request->conn->fd, request->conn->bounded_host->id, strlen(request->conn->bounded_host->id));
	conn_write(request->conn->fd, "\n", 1);
	
	return OK;
}

response_t SRV_GETSPEC (Request* request) {
	system("lscpu > build.spec");
	FILE* lspcu_fp = fopen("build.spec", "r");
	int symbol;
	if (lspcu_fp != NULL) {
		while ((symbol = getc (lspcu_fp)) != EOF) {
			conn_write(request->conn->fd, &symbol, sizeof(char));
		}
		fclose(lspcu_fp);
	}
	remove("build.spec");
	
	return OK;
}

response_t SRV_GETTEMPLATES (Request* request) {
	char __n[16];
	sprintf(__n, "%d", (int) request->conn->parent->templates->n);
	conn_write(request->conn->fd, &__n, strlen(__n));
	
	int i;
	for (i = 0; i != request->conn->parent->templates->n; i++) {
		conn_write(request->conn->fd, "\n", 1);
		
		char* b = (*(HostTemplate**) vector_get(request->conn->parent->templates, i))->id;
		conn_write(request->conn->fd, b, strlen(b));
	}
	conn_write(request->conn->fd, "\n", 1);
	return OK;
}

response_t SRV_TEMPLATE_CREATE (Request* request) {
	CHECK_STRUCTURES({STRCT_TEMPLATECREATE})
	
	HostTemplate in_data;
	in_data.arch = request->structures[0].tc.arch;
	in_data.cflags = request->structures[0].tc.cflags;
	in_data.chost = request->structures[0].tc.chost;
	in_data.extra_c = request->structures[0].tc.make_extra_c;
	
	for (int i = 0; i < in_data.extra_c; i++) {
		in_data.extras[i].select = (template_selects)request->structures[0].tc.extras[i].select;
		in_data.extras[i].make_extra = request->structures[0].tc.extras[i].make_extra;
	}
	
	
	host_template_add(request->conn->parent, &in_data);
	
	return OK;
}

response_t SRV_STAGE_NEW (Request* request) {
	/* We need to bind template using
	 * its index instead of ID
	 * ID in this case is a name not
	 * random generated str
	 */
	
	CHECK_STRUCTURES({STRCT_TEMPLATESELECT})
	
	HostTemplate* t = stage_new(request->conn->parent, request->structures[0].ss.index);
	
	small_map_insert(t->parent->stages, t->new_id, t);
	
	conn_write(request->conn->fd, t->new_id, strlen(t->new_id));
	conn_write(request->conn->fd, "\n", 1);
	return OK;
}

response_t SRV_STAGE (Request* request) {
	CHECK_STRUCTURES({STRCT_HOSTSELECT, STRCT_STAGECOMMAND}) // Host_id is actually a stage
	
	HostTemplate* t = small_map_get(request->conn->parent->stages,
			request->structures[0].hs.host_id);
	if (t == NULL)
		return NOT_FOUND;
	
	int backup_conn, backup_stdout;
	backup_stdout = prv_pipe_to_client(request->conn->fd, &backup_conn);
	
	
	response_t res = OK;
	int command = request->structures[1].sc.command;
	
	char* fname = NULL;
	if (command & STAGE_DOWNLOAD) {
		fname = host_template_download(t);
		if (fname == NULL)
			res = INTERNAL_ERROR;
	}
	if (command & STAGE_EXTRACT) {
		if (fname == NULL)
			fname = host_template_download(t);
		res = host_template_extract(t, fname);
	}
	
	fflush(stdout);
	prv_pipe_back(&request->conn->fd, backup_stdout, backup_conn);
	return res;
}

response_t SRV_GETSTAGED (Request* request) {
	char __n[16];
	sprintf(__n, "%d", (int) request->conn->parent->stages->n);
	conn_write(request->conn->fd, &__n, strlen(__n));
	conn_write(request->conn->fd, "\n", 1);
	
	int i;
	for (i = 0; i != request->conn->parent->stages->n; i++) {
		HostTemplate* __t = (*(HostTemplate***) vector_get(request->conn->parent->stages, i))[1];
		
		conn_write(request->conn->fd, __t->new_id, strlen(__t->new_id));
		conn_write(request->conn->fd, "\n", 1);
	}
	
	return OK;
}

response_t SRV_GETSTAGE (Request* request) {
	CHECK_STRUCTURES({STRCT_TEMPLATESELECT})
	
	HostTemplate* __t = small_map_get_index(request->conn->parent->stages, request->structures[0].ss.index);
	
	if (__t == NULL)
		return NOT_FOUND;
	
	char* buf;
	asprintf(&buf, "%s\n%s\n%s\n%s\n%d\n",
			 __t->new_id,
			 __t->id,
			 __t->cflags,
			 __t->chost,
			 __t->extra_c
	);
	
	conn_write(request->conn->fd, buf, strlen(buf));
	free(buf);
	
	int j;
	for (j = 0; j != __t->extra_c; j++) {
		asprintf(&buf, "%s %d\n", __t->extras[j].make_extra, __t->extras[j].select);
		conn_write(request->conn->fd, buf, strlen(buf));
		free(buf);
	}
	
	return OK;
}

response_t SRV_HANDOFF (Request* request) {
	CHECK_STRUCTURES({STRCT_HOSTSELECT})
	printf ("%s\n", request->structures[0].hs.host_id);
	fflush(stdout);
	HostTemplate* __t = small_map_get(request->conn->parent->stages, request->structures[0].hs.host_id);
	if (__t == NULL)
		return NOT_FOUND;
	
	Host* new_host = host_template_handoff(__t);
	if (!new_host)
		return INTERNAL_ERROR;
	
	vector_add(request->conn->parent->hosts, &new_host);
	write_server(request->conn->parent);
	
	return OK;
}

response_t SRV_SAVE (Request* request) {
	write_server(request->conn->parent);
	return OK;
}

response_t EXIT (Request* request) {
	request->conn->parent->keep_alive = 0;
	return OK;
}

response_t BIN_SERVER (Request* request) {
	FILE* fp = fdopen(request->conn->fd, "wb");
	write_server_fp(request->conn->parent, fp);
	fflush(fp);
	
	return NONE;
}

response_t SRV_HOSTWRITE (Request* request) {
	if (request->conn->bounded_host == NULL)
		return FORBIDDEN;
	
	host_write_make_conf(request->conn->bounded_host);
	
	char* profile_dest;
	char* profile_src;
	struct stat __sym_buff;
	
	asprintf(&profile_src, "/usr/portage/profiles/%s/", request->conn->bounded_host->profile);
	asprintf(&profile_dest, "%s/%s/etc/portage/make.profile", request->conn->parent->location, request->conn->bounded_host->id);
	
	if (lstat(profile_dest, &__sym_buff) == 0) {
		unlink(profile_dest);
	}
	
	linfo("ls to %s", profile_src);
	if (symlink(profile_src, profile_dest) != 0) {
		free(profile_dest);
		free(profile_src);
		lwarning("Failed to symlink %s!", profile_dest);
		return INTERNAL_ERROR;
	}
	
	free(profile_dest);
	free(profile_src);
	
	char* new_dirs[] = {
			request->conn->bounded_host->pkgdir,
			request->conn->bounded_host->port_logdir,
			request->conn->bounded_host->portage_tmpdir,
			"usr/portage/",
			NULL
	};
	
	
	char* curr;
	for (curr = new_dirs[0]; curr != NULL; curr++) {
		char* autogentoo_tmp;
		asprintf(&autogentoo_tmp, "%s/%s/%s", request->conn->parent->location, request->conn->bounded_host->id, curr);
		mkdir(autogentoo_tmp, 0700);
		free(autogentoo_tmp);
	}
	
	char* autogentoo_tmp;
	asprintf(&autogentoo_tmp, "%s/%s/etc/resolv.conf", request->conn->parent->location, request->conn->bounded_host->id);
	file_copy("/etc/resolv.conf", autogentoo_tmp);
	free(autogentoo_tmp);
	
	return OK;
}

response_t SRV_HOSTADVEDIT (Request* request) {
	if (request->struct_c < 2 || request->types[0] != STRCT_HOSTSELECT)
		return BAD_REQUEST;
	Host* selected = server_host_search (request->conn->parent, request->structures[0].hs.host_id);
	
	for (int i = 1; i < request->struct_c; i++) {
		if (request->structures[i].ho.offset_index > sizeof (host_valid_offset) / sizeof (host_valid_offset[0])) {
			if (request->types[i] != STRCT_HOSTOFFSET)
				return BAD_REQUEST;
			return BAD_REQUEST;
		}
		
		host_offset_t offset_item = host_valid_offset[request->structures[i].ho.offset_index];
		if (offset_item.type == VOIDTYPE_STRING)
			*(char**)(selected + offset_item.offset) = strdup (request->structures[i].ho.data);
		else if (offset_item.type == VOIDTYPE_INT)
			*(int*)(selected + offset_item.offset) = *(int*)(request->structures[i].ho.data);
		else if (offset_item.type == VOIDTYPE_STRINGVECTOR) {
			size_t off = 0;
			int j = 0;
			for (char* temp = strdup (request->structures[i].ho.data);
				 strlen(temp);
				 temp = strdup (request->structures[i].ho.data + off), j++)
				string_vector_set((StringVector*)(selected + offset_item.offset), temp, j);
		}
	}
	
	return OK;
}

response_t SRV_HOSTUPLOAD(Request* request) {
	CHECK_STRUCTURES ({STRCT_HOSTSELECT, STRCT_RAW})
	
	FILE* f = fmemopen (request->structures[1].raw.data, request->structures[1].raw.n, "r");
	
	Server* srv = request->conn->parent;
	Host* target = read_host(f);
	target->parent = srv;
	
	Host** out_pos = NULL;
	
	int i;
	for (i = 0; i < srv->hosts->n; i++) {
		Host** t = (Host**)vector_get (srv->hosts, i);
		if (strcmp ((*t)->id, request->structures[0].hs.host_id) == 0) {
			out_pos = t;
			break;
		}
	}
	
	if (out_pos == NULL)
		vector_add (request->conn->parent->hosts, &target);
	else {
		host_free (*out_pos);
		*out_pos = target;
	}
	
	fclose (f);
	return OK;
}

response_t BIN_QUEUE(Request* request) {
	queue_write(request->conn->parent->queue->head, request->conn->fd);
	queue_free(request->conn->parent->queue->head);
	
	return NONE;
}

response_t WORKER_HANDOFF(Request* request) {
	CHECK_STRUCTURES({STRCT_HOSTSELECT, STRCT_WORKERRESPONSE});
	
	if (request->structures[1].wr.response != 0)
		return OK;
	
	HostTemplate* src = small_map_delete(request->conn->parent->stages, request->structures[0].hs.host_id);
	host_template_free(src);
	
	return OK;
}

response_t WORKER_MAKECONF(Request* request) {
	return OK;
}