#include <stdlib.h>
#include <string.h>
#include <autogentoo/autogentoo.h>
#include <netinet/in.h>
#include <autogentoo/user.h>
#include <errno.h>
#include <autogentoo/host_environment.h>
#include <autogentoo/writeconfig.h>
#include <semaphore.h>

void* read_void(size_t len, FILE* fp) {
	void* out = malloc (len);
	fread(out, 1, len, fp);
	return out;
}

size_t write_void(void* ptr, size_t len, FILE* fp) {
	return fwrite(ptr, 1, len, fp);
}

size_t write_int(int src, FILE* fp) {
	int big_endian_src = htonl((uint32_t) src);
	
	return fwrite(&big_endian_src, sizeof(int), 1, fp);
}

int read_int(FILE* fp) {
	int out;
	fread(&out, sizeof(int), 1, fp);
	out = ntohl((uint32_t) out);
	return out;
}

int read_int_fd(int fd) {
	int out;
	read(fd, &out, sizeof (int));
	return ntohl((uint32_t) out);
}

ssize_t write_int_fd(int fd, int i) {
	int to_send = htonl((uint32_t) i);
	return write(fd, &to_send, sizeof(int));
}

size_t write_server(Server* server) {
	char* config_file_name = ".autogentoo.config";
	char* config_file = malloc(strlen(server->location) + strlen(config_file_name) + 2);
	sprintf(config_file, "%s/%s", server->location, config_file_name);
	
	
	FILE* to_write = fopen(config_file, "wb+");
	worker_lock(fileno(to_write));
	
	if (to_write == NULL) {
		lerror("Failed to open '%s' for writing-----------", config_file);
		lerror("Error [%d] %s", errno, strerror(errno));
		
		char cwd[PATH_MAX];
		if (getcwd(cwd, sizeof(cwd)) != NULL)
			lerror("[%s]", cwd);
		else
			perror("getcwd() error");
		
		return 0;
	}
	free(config_file);
	
	size_t size = write_server_fp(server, to_write);
	
	
	worker_unlock(fileno(to_write));
	fclose(to_write);
	
	
	return size;
}

size_t write_server_fp(Server* server, FILE* fp) {
	size_t size = 0;
	
	int i;
	for (i = 0; i != server->hosts->n; i++) {
		size += write_host_fp((Host*) vector_get(server->hosts, i), fp);
	}
	
	StringVector* token_keys = map_all_keys(server->auth_tokens);
	
	for (i = 0; i < token_keys->n; i++) {
		AccessToken* token = map_get(server->auth_tokens, string_vector_get(token_keys, i));
		if (!token) {
			string_vector_free(token_keys);
			lerror("An error occured which writing token to config");
			exit (1);
		}
		
		size += write_access_token_fp(token, fp);
	}
	string_vector_free(token_keys);
	
	size += write_int(AUTOGENTOO_SERVER_TOKEN, fp);
	size += write_string(server->autogentoo_org_token, fp);
	
	size += write_int(AUTOGENTOO_SUDO_TOKEN, fp);
	size += write_string(server->sudo_token, fp);
	
	size += write_int(AUTOGENTOO_FILE_END, fp);
	
	return size;
}

size_t write_access_token_fp(AccessToken* token, FILE* fp) {
	size_t size = 0;
	
	size += write_int(AUTOGENTOO_ACCESS_TOKEN, fp);
	size += write_string(token->auth_token, fp);
	size += write_string(token->user_id, fp);
	size += write_string(token->host_id, fp);
	size += write_int(token->access_level, fp);
	
	return size;
}

size_t write_host_fp(Host* host, FILE* fp) {
	size_t size = 0;
	size += write_int(AUTOGENTOO_HOST, fp);
	size += write_string(host->id, fp);
	size += write_int(CHR_NOT_MOUNTED, fp); // What if system restarted (might as well assume unmounted)
	size += write_int(host->environment_status, fp);
	
	size += write_string(host->hostname, fp);
	size += write_string(host->profile, fp);
	size += write_string(host->arch, fp);
	
	size += write_string(host->environment->cflags, fp);
	size += write_string(host->environment->cxxflags, fp);
	size += write_string(host->environment->distdir, fp);
	size += write_string(host->environment->lc_messages, fp);
	size += write_string(host->environment->pkgdir, fp);
	size += write_string(host->environment->portage_logdir, fp);
	size += write_string(host->environment->portage_tmpdir, fp);
	size += write_string(host->environment->portdir, fp);
	size += write_string(host->environment->use, fp);
	
	size += write_int((int)host->environment->extra->n, fp);
	
	int i;
	for (i = 0; i < host->environment->extra->n; i++) {
		SmallMap_key* current_key = ((SmallMap_key*)vector_get(host->environment->extra, i));
		size += write_string(current_key->key, fp);
		size += write_string(current_key->data_ptr, fp);
	}
	
	
	if (host->kernel) {
		for (i = 0; i != host->kernel->n; i++) {
			size += write_int(AUTOGENTOO_HOST_KERNEL, fp);
			
			Kernel* current_kernel = (Kernel*)vector_get(host->kernel, i);
			size += write_string(current_kernel->kernel_target, fp);
			size += write_string(current_kernel->version, fp);
		}
	}
	
	size += write_int(AUTOGENTOO_HOST_END, fp);
	
	if (host_setstatus(host) != 0)
		lerror("Failed to set status for %s", host->id);
	
	return size;
}

Server* read_server(char* location, char* port, server_t opts) {
	char* config_file_name = ".autogentoo.config";
	char* config_file = malloc(strlen(location) + strlen(config_file_name) + 2);
	sprintf(config_file, "%s/%s", location, config_file_name);
	
	FILE* fp = fopen(config_file, "rb");
	free(config_file);
	Server* out = server_new(location, port, opts);
	if (fp == NULL)
		return out;
	
	Host* host_temp;
	AccessToken* token_temp;
	
	//size_t len = (size_t)read_int(fp);
	
	int current = read_int(fp);
	while (current != AUTOGENTOO_FILE_END) {
		switch (current) {
			case AUTOGENTOO_HOST:
				host_temp = read_host(fp);
				host_temp->parent = out;
				vector_add(out->hosts, host_temp);
				break;
			case AUTOGENTOO_SERVER_TOKEN:
				out->autogentoo_org_token = read_string(fp);
				break;
			case AUTOGENTOO_ACCESS_TOKEN:
				token_temp = read_access_token(fp);
				map_insert(out->auth_tokens, token_temp->auth_token, token_temp);
				break;
			case AUTOGENTOO_SUDO_TOKEN:
				out->sudo_token = read_string(fp);
				break;
			default:
				lerror("Could not understand autogentoo data type: 0x%x", current);
				exit(1);
		}
		
		current = read_int(fp);
	}
	
	fclose(fp);
	
	return out;
}

AccessToken* read_access_token(FILE* fp) {
	AccessToken* token = malloc(sizeof(AccessToken));
	
	token->auth_token = read_string(fp);
	token->user_id = read_string(fp);
	token->host_id = read_string(fp);
	token->access_level = read_int(fp);
	
	return token;
}

Host* read_host(FILE* fp) {
	Host* out = malloc(sizeof(Host));
	
	out->id = read_string(fp);
	out->chroot_status = (chroot_t)read_int(fp);
	out->environment_status = (host_env_t)read_int(fp);
	
	out->hostname = read_string(fp);
	out->profile = read_string(fp);
	out->arch = read_string(fp);
	
	out->environment = host_environment_new(out);
	
	
	out->environment->cflags = read_string(fp);
	out->environment->cxxflags = read_string(fp);
	out->environment->distdir = read_string(fp);
	out->environment->lc_messages = read_string(fp);
	out->environment->pkgdir = read_string(fp);
	out->environment->portage_logdir = read_string(fp);
	out->environment->portage_tmpdir = read_string(fp);
	out->environment->portdir = read_string(fp);
	out->environment->use = read_string(fp);
	
	int n, i;
	n = read_int(fp);
	char* temp_name, *temp_val;
	
	for (i = 0; i != n; i++) {
		temp_name = read_string(fp);
		temp_val = read_string(fp);
		small_map_insert(out->environment->extra, temp_name, temp_val);
		
		/* Dont free temp_val, we never dup it */
		free(temp_name);
	}
	
	host_init_extras(out);
	
	int current = 0;
	while (current != AUTOGENTOO_HOST_END) {
		Kernel* new_kernel;
		AccessToken new_token;
		current = read_int(fp);
		switch (current) {
			case AUTOGENTOO_HOST_KERNEL:
				new_kernel = malloc(sizeof(Kernel));
				new_kernel->kernel_target = read_string(fp);
				new_kernel->version = read_string(fp);
				vector_add(out->kernel, new_kernel);
				break;
			case AUTOGENTOO_HOST_END:
				break;
			default:
				lerror("Could not understand autogentoo data type: 0x%x", current);
				break;
		}
	}
	
	return out;
}

size_t write_string(char* src, FILE* fp) {
	size_t len;
	if (!src)
		len = 0;
	else
		len = strlen(src);
	write_int((int)len, fp);
	
	if (len)
		fwrite(src, 1, len, fp);
	
	return len + sizeof(size_t);
}

char* read_string(FILE* fp) {
	size_t len = (size_t)read_int(fp);
	
	if (!len)
		return NULL;
	
	char* out = malloc(len + 1);
	
	if (!out) {
		lerror("Failed to allocate memory with size %d", len + 1);
		exit(1);
	}
	
	fread(out, 1, len, fp);
	out[len] = 0;
	
	return out;
}
