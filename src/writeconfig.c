#include <stdlib.h>
#include <string.h>
#include <autogentoo/autogentoo.h>
#include <netinet/in.h>
#include <autogentoo/user.h>

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
	
	for (i = 0; i != server->host_bindings->n; i++) {
		size += write_host_binding_fp((HostBind*) vector_get(server->host_bindings, i), fp);
	}
	
	for (i = 0; i != server->stages->n; i++) {
		void** __t = *(void***) vector_get(server->stages, i);
		size += write_stage_fp(__t[1], fp);
	}
	for (i = 0; i != server->templates->n; i++) {
		void* __t = *vector_get(server->templates, i);
		size += write_template_fp(__t, fp);
	}
	
	size += write_int(AUTOGENTOO_FILE_END, fp);
	
	return size;
}

size_t write_host_fp(Host* host, FILE* fp) {
	size_t size = 0;
	size += write_int(AUTOGENTOO_HOST, fp);
	size += write_string(host->id, fp);
	size += write_int(CHR_NOT_MOUNTED, fp); // What if system restarted (might as well assume unmounted)
	
	size += write_string(host->hostname, fp);
	size += write_string(host->profile, fp);
	size += write_string(host->cflags, fp);
	size += write_string(host->cxxflags, fp);
	size += write_string(host->use, fp);
	size += write_string(host->arch, fp);
	size += write_string(host->chost, fp);
	
	size += write_int((int)host->extra->n, fp);
	
	int i;
	for (i = 0; i != host->extra->n; i++) {
		size += write_string(string_vector_get(host->extra, i), fp);
	}
	
	size += write_string(host->portage_tmpdir, fp);
	size += write_string(host->portdir, fp);
	size += write_string(host->distdir, fp);
	size += write_string(host->pkgdir, fp);
	size += write_string(host->port_logdir, fp);
	
	if (host->kernel) {
		for (i = 0; i != host->kernel->n; i++) {
			size += write_int(AUTOGENTOO_HOST_KERNEL, fp);
			
			Kernel* current_kernel = *(Kernel**)vector_get(host->kernel, i);
			size += write_string(current_kernel->kernel_target, fp);
			size += write_string(current_kernel->version, fp);
		}
	}
	
	if (host->auth_tokens)
		for (i = 0; i != host->auth_tokens->n; i++) {
			size += write_int(AUTOGENTOO_ACCESS_TOKEN, fp);
			
			AccessToken current_token = *(AccessToken*)vector_get(host->auth_tokens, i);
			
			size += write_string(current_token.user_id, fp);
			size += write_void(current_token.auth_token, AUTOGENTOO_TOKEN_LENGTH, fp);
			size += write_int(current_token.access_level, fp);
		}
	
	size += write_int(AUTOGENTOO_HOST_END, fp);
	
	return size;
}

size_t write_host_binding_fp(HostBind* bind, FILE* fp) {
	size_t size = 0;
	size += write_int(AUTOGENTOO_HOST_BINDING, fp);
	size += write_string(bind->host->id, fp);
	size += write_string(bind->ip, fp);
	
	return size;
}

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

Server* read_server(char* location, char* port, server_t opts) {
	char* config_file_name = ".autogentoo.config";
	char* config_file = malloc(strlen(location) + strlen(config_file_name) + 2);
	sprintf(config_file, "%s/%s", location, config_file_name);
	
	FILE* fp = fopen(config_file, "rb");
	if (fp == NULL) {
		free(config_file);
		Server* out = server_new(location, port, opts);
		host_template_list_init(out);
		return out;
	}
	
	free(config_file);
	Server* out = server_new(location, port, opts);
	
	Host* host_temp;
	HostBind bind_temp;
	HostTemplate* stage_temp;
	
	int current = 0;
	while (current != AUTOGENTOO_FILE_END) {
		current = read_int(fp);
		
		switch (current) {
			case AUTOGENTOO_HOST:
				host_temp = read_host(fp);
				host_temp->parent = out;
				vector_add(out->hosts, &host_temp);
				break;
			case AUTOGENTOO_HOST_BINDING:
				read_host_binding(out, &bind_temp, fp);
				vector_add(out->host_bindings, &bind_temp);
				break;
			case AUTOGENTOO_STAGE:
				stage_temp = malloc(sizeof(HostTemplate));
				read_stage(out, stage_temp, fp);
				small_map_insert(out->stages, stage_temp->new_id, stage_temp);
				break;
			case AUTOGENTOO_TEMPLATE:
				stage_temp = malloc(sizeof(HostTemplate));
				read_template (out, stage_temp, fp);
				vector_add(out->templates, &stage_temp);
			case AUTOGENTOO_FILE_END:
				break;
			default:
				lerror("Could not understand autogentoo data type: 0x%x", current);
				break;
		}
	}
	
	fclose(fp);
	
	return out;
}

Host* read_host(FILE* fp) {
	Host* out = malloc(sizeof(Host));
	
	out->id = read_string(fp);
	out->chroot_status = (chroot_t)read_int(fp);
	
	out->hostname = read_string(fp);
	out->profile = read_string(fp);
	out->cflags = read_string(fp);
	out->cxxflags = read_string(fp);
	out->use = read_string(fp);
	out->arch = read_string(fp);
	out->chost = read_string(fp);
	
	out->extra = string_vector_new();
	int n, i;
	n = read_int(fp);
	for (i = 0; i != n; i++) {
		char* temp = read_string(fp);
		string_vector_add(out->extra, temp);
		free(temp);
	}
	
	out->portage_tmpdir = read_string(fp);
	out->portdir = read_string(fp);
	out->distdir = read_string(fp);
	out->pkgdir = read_string(fp);
	out->port_logdir = read_string(fp);
	
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
			case AUTOGENTOO_ACCESS_TOKEN:
				new_token.user_id = read_string(fp);
				new_token.auth_token = read_void(AUTOGENTOO_TOKEN_LENGTH, fp);
				new_token.access_level = read_int(fp);
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
	dest->host = server_host_search(server, _host_id);
	dest->ip = read_string(fp);
	free(_host_id);
}

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

inline void* read_void(size_t len, FILE* fp) {
	void* out = malloc (len);
	fread(out, 1, len, fp);
	return out;
}

inline size_t write_void(void* ptr, size_t len, FILE* fp) {
	return fwrite(ptr, 1, len, fp);
}

inline size_t write_int(int src, FILE* fp) {
	int big_endian_src = htonl((uint32_t) src);
	
	return fwrite(&big_endian_src, sizeof(int), 1, fp);
}

inline int read_int(FILE* fp) {
	int out;
	fread(&out, sizeof(int), 1, fp);
	out = ntohl((uint32_t) out);
	return out;
}