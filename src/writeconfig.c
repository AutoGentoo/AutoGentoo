#include <stdlib.h>
#include <string.h>
#include <autogentoo/autogentoo.h>
#include <netinet/in.h>
#include <autogentoo/user.h>

inline size_t rsa_write_void(size_t len, FILE* fp) {
	return 0;
}

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
	if (to_write == NULL) {
		lerror("Failed to open '%s' for writing!", config_file);
		exit(1);
	}
	
	free(config_file);
	size_t size = write_server_fp(server, to_write);
	
	fclose(to_write);
	
	return size;
}

size_t write_server_fp(Server* server, FILE* fp) {
	size_t size = 0;
	
	int i;
	for (i = 0; i != server->hosts->n; i++) {
		size += write_host_fp(*(Host**) vector_get(server->hosts, i), fp);
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
	
	size += write_string(host->hostname, fp);
	size += write_string(host->profile, fp);
	size += write_string(host->arch, fp);
	
	size += write_int((int)host->make_conf->n, fp);
	
	int i;
	for (i = 0; i != host->make_conf->n; i++) {
		size += write_string((*(SmallMap_key**)vector_get(host->make_conf, i))->key, fp);
		size += write_string((*(SmallMap_key**)vector_get(host->make_conf, i))->data_ptr, fp);
	}
	
	if (host->kernel) {
		for (i = 0; i != host->kernel->n; i++) {
			size += write_int(AUTOGENTOO_HOST_KERNEL, fp);
			
			Kernel* current_kernel = *(Kernel**)vector_get(host->kernel, i);
			size += write_string(current_kernel->kernel_target, fp);
			size += write_string(current_kernel->version, fp);
		}
	}
	
	size += write_int(AUTOGENTOO_HOST_END, fp);
	
	return size;
}

/*
size_t write_stage_fp(HostTemplate* temp, FILE* fp) {
	size_t size = 0;
	size += write_int(AUTOGENTOO_STAGE, fp);
	size += write_string(temp->id, fp);
	size += write_string(temp->arch, fp);
	size += write_string(temp->cflags, fp);
	size += write_string(temp->chost, fp);
	
	size += write_int(temp->extra_c, fp);
	int i;
	for (i = 0; i != temp->extra_c; i++) {
		size += write_string(temp->extras[i].make_extra, fp);
		size += write_int(temp->extras[i].select, fp);
	}
	
	size += write_string(temp->dest_dir, fp);
	size += write_string(temp->new_id, fp);
	
	return size;
}

size_t write_template_fp (HostTemplate* temp, FILE* fp) {
	size_t size = 0;
	size += write_int(AUTOGENTOO_TEMPLATE, fp);
	size += write_string(temp->id, fp);
	size += write_string(temp->arch, fp);
	size += write_string(temp->cflags, fp);
	size += write_string(temp->chost, fp);
	
	size += write_int(temp->extra_c, fp);
	int i;
	for (i = 0; i != temp->extra_c; i++) {
		size += write_string(temp->extras[i].make_extra, fp);
		size += write_int(temp->extras[i].select, fp);
	}
	
	return size;
}
*/

Server* read_server(char* location, char* port, server_t opts) {
	char* config_file_name = ".autogentoo.config";
	char* config_file = malloc(strlen(location) + strlen(config_file_name) + 2);
	sprintf(config_file, "%s/%s", location, config_file_name);
	
	FILE* fp = fopen(config_file, "rb");
	if (fp == NULL) {
		free(config_file);
		Server* out = server_new(location, port, opts);
		return out;
	}
	
	free(config_file);
	Server* out = server_new(location, port, opts);
	
	Host* host_temp;
	AccessToken* token_temp;
	
	int current = 0;
	int __break = 0;
	while (current != AUTOGENTOO_FILE_END) {
		current = read_int(fp);
		if (__break)
			break;
		
		switch (current) {
			case AUTOGENTOO_HOST:
				host_temp = read_host(fp);
				host_temp->parent = out;
				vector_add(out->hosts, &host_temp);
				break;
			case AUTOGENTOO_FILE_END:
				__break = 1;
				break;
			case AUTOGENTOO_SERVER_TOKEN:
				out->autogentoo_org_token = read_string(fp);
				break;
			case AUTOGENTOO_ACCESS_TOKEN:
				token_temp = read_access_token(fp);
				map_insert(out->auth_tokens, token_temp->auth_token, token_temp);
				break;
			default:
				lerror("Could not understand autogentoo data type: 0x%x", current);
				__break = 1;
				break;
		}
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
	
	out->hostname = read_string(fp);
	out->profile = read_string(fp);
	out->arch = read_string(fp);
	
	out->make_conf = small_map_new(20, 5);
	int n, i;
	n = read_int(fp);
	char* temp_name, *temp_val;
	
	for (i = 0; i != n; i++) {
		temp_name = read_string(fp);
		temp_val = read_string(fp);
		small_map_insert(out->make_conf, temp_name, temp_val);
		
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
			//case AUTOGENTOO_ACCESS_TOKEN:
			//	new_token.user_id = read_string(fp);
			//	//new_token.auth_token = read_void(AUTOGENTOO_TOKEN_LENGTH, fp);
			//	new_token.access_level = read_int(fp);
			case AUTOGENTOO_HOST_END:
				break;
			default:
				lerror("Could not understand autogentoo data type: 0x%x", current);
				break;
		}
	}
	
	return out;
}

void read_host_binding(Server* server, HostBind* dest, FILE* fp) {
	char* _host_id = read_string(fp);
	dest->host = server_get_host(server, _host_id);
	dest->ip = read_string(fp);
	free(_host_id);
}

/*
void read_stage(Server* server, HostTemplate* dest, FILE* fp) {
	dest->parent = server;
	dest->id = read_string(fp);
	dest->arch = read_string(fp);
	dest->cflags = read_string(fp);
	dest->chost = read_string(fp);
	
	dest->extra_c = read_int(fp);
	int i;
	for (i = 0; i != dest->extra_c; i++) {
		dest->extras[i].make_extra = read_string(fp);
		dest->extras[i].select = (template_selects)read_int(fp);
	}
	
	dest->dest_dir = read_string(fp);
	dest->new_id = read_string(fp);
}

void read_template(Server* server, HostTemplate* dest, FILE* fp) {
	dest->parent = server;
	dest->id = read_string(fp);
	dest->arch = read_string(fp);
	dest->cflags = read_string(fp);
	dest->chost = read_string(fp);
	
	dest->extra_c = read_int(fp);
	int i;
	for (i = 0; i != dest->extra_c; i++) {
		dest->extras[i].make_extra = read_string(fp);
		dest->extras[i].select = (template_selects)read_int(fp);
	}
	
	dest->dest_dir = NULL;
	dest->new_id = NULL;
}
*/

size_t write_string(char* src, FILE* fp) {
	if (!src) {
		fputc(0, fp);
		return 1;
	}
	
	fputs(src, fp);
	fputc(0, fp);
	
	return strlen(src) + 1;
}

char* read_string(FILE* fp) {
	int i, c;
	for (i = 0, c = fgetc(fp); c != 0; i++, c = fgetc(fp));
	fseek(fp, -i - 1, SEEK_CUR);
	
	char* out = NULL;
	if ((size_t) i + 1) {
		out = malloc((size_t) i + 1);
		fread(out, 1, (size_t) i + 1, fp);
	}
	
	return out;
}
