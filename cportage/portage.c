//
// Created by atuser on 4/23/19.
//

#define _GNU_SOURCE

#include <sys/stat.h>
#include <errno.h>
#include "portage.h"
#include "portage_log.h"
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include "emerge.h"
#include "package.h"

int portage_get_hash_fd(sha_hash* target, int fd, const EVP_MD* algorithm) {
	FILE* hash = fdopen(fd, "r");
	*target = malloc(EVP_MAX_MD_SIZE);
	
	EVP_MD_CTX mdctx;
	unsigned int md_len;
	EVP_MD_CTX_init(&mdctx);
	EVP_DigestInit_ex(&mdctx, algorithm, NULL);
	
	char chunk[64];
	ssize_t current_bytes = 0;
	while ((current_bytes = fread(chunk, 64, 1, hash)) > 0) {
		EVP_DigestUpdate(&mdctx, chunk, current_bytes);
	}
	
	
	EVP_DigestFinal_ex(&mdctx, *target, &md_len);
	EVP_MD_CTX_cleanup(&mdctx);
	
	return (int)md_len;
}

int portage_get_hash(sha_hash* target, char* path, const EVP_MD* algorithm) {
	struct stat path_stat;
	if (stat(path, &path_stat) != 0) {
		plog_warn("hash failed %s - %s [%d]", path, strerror(errno), errno);
		return -1;
	}
	
	int fd = open(path, O_RDONLY);
	return portage_get_hash_fd(target, fd, algorithm);
}

Repository* repository_new() {
	Repository* out = malloc(sizeof(Repository));
	
	out->packages = map_new(65536, 0.8);
	out->location = NULL;
	
	out->masters = NULL;
	out->priority = -1000;
	out->strict_misc_digests = 1;
	out->sync_allow_hardlinks = 1;
	out->sync_openpgp_key_path = strdup("/usr/share/openpgp-keys/gentoo-release.asc");
	out->sync_openpgp_key_refresh_retry_count = 40;
	out->sync_openpgp_key_refresh_retry_delay_exp_base = 2;
	out->sync_openpgp_key_refresh_retry_delay_max = 60;
	out->sync_openpgp_key_refresh_retry_delay_mult = 4;
	out->sync_openpgp_key_refresh_retry_overall_timeout = 1200;
	out->sync_rcu = 0;
	out->sync_type = strdup("rsync");
	out->sync_uri = strdup("rsync://rsync.gentoo.org/gentoo-portage");
	out->sync_rsync_verify_max_age = 24;
	out->sync_rsync_verify_jobs = 1;
	
	out->sync_rsync_extra_opts = NULL;
	out->sync_rsync_verify_metamanifest = 0;
	
	out->auto_sync = 1;
	
	return out;
}

char** strsplt(char* to_split, size_t* n) {
	StringVector* sv_out = string_vector_new();
	string_vector_split(sv_out, to_split, " ");
	
	char** out = sv_out->ptr;
	*n = sv_out->n;
	
	return out;
}

Repository* emerge_repos_conf(Emerge* emerge) {
	char* repos_conf_path = NULL;
	asprintf(&repos_conf_path, "%s/etc/portage/repos.conf", emerge->root);
	
	FILE* fp = fopen(repos_conf_path, "r");
	free(repos_conf_path);
	if (!fp) {
		plog_error("Failed to open repos.conf");
		return NULL;
	}
	
	Repository* parent = NULL;
	Repository* current_repo = NULL;
	Repository* next_repo = NULL;
	
	ssize_t read = 0;
	size_t s;
	char* line = NULL;
	while ((read = getline(&line, &s, fp)) > 0) {
		if (line[0] == '[') {
			(*strchr(line, ']')) = 0;
			char* current_name = strdup(line + 1);
			
			if (strcmp(current_name, "DEFAULT") != 0) {
				next_repo = repository_new();
				if (parent)
					current_repo->next = next_repo;
				else
					parent = next_repo;
				
				current_repo = next_repo;
				current_repo->name = current_name;
			}
			else
				free(current_name);
			
			continue;
		}
		
		char* name = strtok(line, "=");
		char* value = strtok(NULL, "\n");
		
		if (!name || !value) {
			continue;
		}
		
		for(; *name && *name == ' '; name++);
		for(; *value && *value == ' '; value++);
		
		size_t n = strlen(name) - 1;
		for (; name[n] == ' ' && n >= 0; n--) {
			name[n] = 0;
		}
		
		n = strlen(value) - 1;
		for (; (value[n] == ' ' || value[n] == '\n') && n >= 0; n--)
			value[n] = 0;
		
		if (!current_repo) {
			if (strcmp(name, "main-repo") == 0)
				emerge->default_repo = strdup(value);
			continue;
		}
		
		if (strcmp(name, "masters") == 0) {
			n = 0;
			current_repo->masters = strsplt(value, &n);
			current_repo->masters[n] = NULL;
		}
		else if (strcmp(name, "location") == 0) {
			if (current_repo->location)
				free(current_repo->location);
			current_repo->location = strdup(value);
		}
		else if (strcmp(name, "priority") == 0)
			current_repo->priority = atoi(value);
		else if (strcmp(name, "strict-misc-digests") == 0)
			current_repo->strict_misc_digests = atoi(value);
		else if (strcmp(name, "sync-allow-hardlinks") == 0)
			current_repo->sync_allow_hardlinks = atoi(value);
		else if (strcmp(name, "sync-openpgp-key-path") == 0) {
			if (current_repo->sync_openpgp_key_path)
				free(current_repo->sync_openpgp_key_path);
			current_repo->sync_openpgp_key_path = strdup(value);
		}
		else if (strcmp(name, "sync-openpgp-key-refresh-retry-count") == 0)
			current_repo->sync_openpgp_key_refresh_retry_count = atoi(value);
		else if (strcmp(name, "sync-openpgp-key-refresh-retry-delay-exp-base") == 0)
			current_repo->sync_openpgp_key_refresh_retry_delay_exp_base = atoi(value);
		else if (strcmp(name, "sync-openpgp-key-refresh-retry-delay-max") == 0)
			current_repo->sync_openpgp_key_refresh_retry_delay_max = atoi(value);
		else if (strcmp(name, "sync-openpgp-key-refresh-retry-delay-mult") == 0)
			current_repo->sync_openpgp_key_refresh_retry_delay_mult = atoi(value);
		else if (strcmp(name, "sync-openpgp-key-refresh-retry-overall-timeout") == 0)
			current_repo->sync_openpgp_key_refresh_retry_overall_timeout = atoi(value);
		else if (strcmp(name, "sync-rcu") == 0)
			current_repo->sync_rcu = atoi(value);
		else if (strcmp(name, "sync-type") == 0) {
			if (current_repo->sync_type)
				free(current_repo->sync_type);
			current_repo->sync_type = strdup(value);
		}
		else if (strcmp(name, "sync-uri") == 0) {
			if (current_repo->sync_uri)
				free(current_repo->sync_uri);
			current_repo->sync_uri = strdup(value);
		}
		else if (strcmp(name, "sync-rsync-verify-max-age") == 0)
			current_repo->sync_rsync_verify_max_age = atoi(value);
		else if (strcmp(name, "sync-rsync-verify-jobs") == 0)
			current_repo->sync_rsync_verify_jobs = atoi(value);
		else if (strcmp(name, "sync-rsync-extra-opts") == 0) {
			n = 0;
			current_repo->sync_rsync_extra_opts= strsplt(value, &n);
			current_repo->sync_rsync_extra_opts[n] = NULL;
		}
		else if (strcmp(name, "sync-rsync-verify-metamanifest") == 0)
			current_repo->sync_rsync_verify_metamanifest = atoi(value);
		else if (strcmp(name, "auto_sync") == 0) {
			if (strcmp(value, "true") == 0 || strcmp(value, "yes") == 0)
				current_repo->auto_sync = 1;
			else
				current_repo->auto_sync = 0;
		}
	}
	free(line);
	
	return parent;
}

void repository_free(Repository* repo) {
	Repository* curr = repo;
	Repository* next = NULL;
	
	while (curr) {
		next = curr->next;
		free(curr->name);
		free(curr->location);
		free(curr->sync_openpgp_key_path);
		free(curr->sync_type);
		free(curr->sync_uri);
		
		int i;
		if (curr->masters) {
			for (i = 0; curr->masters[i]; i++)
				free(curr->masters[i]);
			free(curr->masters);
		}
		if (curr->sync_rsync_extra_opts) {
			for (i = 0; curr->sync_rsync_extra_opts[i]; i++)
				free(curr->sync_rsync_extra_opts[i]);
			free(curr->sync_rsync_extra_opts);
		}
		
		for (i = 0; i < curr->category_manifests->n; i++) {
			Manifest* cat_man = *(Manifest**)vector_get(curr->category_manifests, i);
			for (int j = 0; j < cat_man->parsed->n; j++) {
				Manifest* atom_man = *(Manifest**)vector_get(cat_man->parsed, j);
				manifest_free(atom_man);
			}
			vector_free(cat_man->parsed);
			manifest_free(cat_man);
		}
		vector_free(curr->category_manifests);
		
		map_free(curr->packages, (void (*) (void*))package_free);
		
		free(curr);
		curr = next;
	}
}