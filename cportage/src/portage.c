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
#include "emerge.h"
#include "package.h"
#include "directory.h"
#include "compress.h"
#include "ebuild/cache.h"
#include <openssl/evp.h>

int portage_get_hash_fd(sha_hash* target, int fd, const EVP_MD* algorithm) {
	FILE* hash = fdopen(fd, "r");
	*target = malloc(EVP_MAX_MD_SIZE);


#if (OPENSSL_VERSION_NUMBER & 0x00ff000fL) == 0x0010000fL
	EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
#elif (OPENSSL_VERSION_NUMBER & 0x00ff000fL) == 0x0000000fL
	EVP_MD_CTX* mdctx = EVP_MD_CTX_create();
#endif
	
	
	unsigned int md_len;
	EVP_DigestInit_ex(mdctx, algorithm, NULL);
	
	char chunk[64];
	ssize_t current_bytes = 0;
	while ((current_bytes = fread(chunk, 64, 1, hash)) > 0) {
		EVP_DigestUpdate(mdctx, chunk, current_bytes);
	}
	
	EVP_DigestFinal_ex(mdctx, *target, &md_len);

#if (OPENSSL_VERSION_NUMBER & 0x00ff000fL) == 0x0010000fL
	EVP_MD_CTX_free(mdctx);
#elif (OPENSSL_VERSION_NUMBER & 0x00ff000fL) == 0x0000000fL
	EVP_MD_CTX_destroy(mdctx);
#endif
	
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
	out->categories = vector_new(VECTOR_REMOVE | VECTOR_ORDERED);
	out->categories_names = string_vector_new();
	out->location = NULL;
	out->next = NULL;
	
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
	
	char** out = (char**)sv_out->ptr;
	*n = sv_out->n;
	
	return out;
}

Repository* emerge_repos_conf(Emerge* emerge) {
	char* repos_conf_path = NULL;
	asprintf(&repos_conf_path, "%s/etc/portage/repos.conf", emerge->root);
	
	Repository* current_repo = NULL;
	
	FPNode* repos_conf = open_directory(repos_conf_path);
	free(repos_conf_path);
	
	if (!repos_conf)
		return NULL;
	
	
	for (FPNode* curr_fp = repos_conf; curr_fp; curr_fp = curr_fp->next) {
		FILE* fp = fopen(curr_fp->path, "r");
		if (!fp) {
			plog_error("Failed to open %s", curr_fp->path);
			fpnode_free(repos_conf);
			return NULL;
		}
		
		size_t s;
		char* line = NULL;
		while (getline(&line, &s, fp) > 0) {
			if (line[0] == '[') {
				(*strchr(line, ']')) = 0;
				char* current_name = strdup(line + 1);
				
				if (strcmp(current_name, "DEFAULT") != 0) {
					Repository* new_repo = repository_new();
					new_repo->next = current_repo;
					new_repo->parent = emerge;
					new_repo->name = current_name;
					
					if (strcmp(emerge->default_repo_name, current_name) == 0)
						emerge->default_repo = new_repo;
					
					current_repo = new_repo;
				}
				else
					free(current_name);
				
				continue;
			}
			
			char* name = strtok(line, "=");
			char* value = strtok(NULL, "\n");
			
			if (!name || !value)
				continue;
			
			for(; *name && *name == ' '; name++);
			for(; *value && *value == ' '; value++);
			
			size_t n = strlen(name) - 1;
			for (; name[n] == ' ' && n >= 0; n--) {
				name[n] = 0;
			}
			
			n = strlen(value) - 1;
			for (; (value[n] == ' ' || value[n] == '\n') && n >= 0; n--)
				value[n] = 0;
			
			if (strcmp(name, "main-repo") == 0) {
				emerge->default_repo_name = strdup(value);
				continue;
			}
			
			if (!current_repo)
				continue;
			
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
				current_repo->priority = (int)strtol(value, NULL, 10);
			else if (strcmp(name, "strict-misc-digests") == 0)
				current_repo->strict_misc_digests = (int)strtol(value, NULL, 10);
			else if (strcmp(name, "sync-allow-hardlinks") == 0)
				current_repo->sync_allow_hardlinks = (int)strtol(value, NULL, 10);
			else if (strcmp(name, "sync-openpgp-key-path") == 0) {
				if (current_repo->sync_openpgp_key_path)
					free(current_repo->sync_openpgp_key_path);
				current_repo->sync_openpgp_key_path = strdup(value);
			}
			else if (strcmp(name, "sync-openpgp-key-refresh-retry-count") == 0)
				current_repo->sync_openpgp_key_refresh_retry_count = (int)strtol(value, NULL, 10);
			else if (strcmp(name, "sync-openpgp-key-refresh-retry-delay-exp-base") == 0)
				current_repo->sync_openpgp_key_refresh_retry_delay_exp_base = (int)strtol(value, NULL, 10);
			else if (strcmp(name, "sync-openpgp-key-refresh-retry-delay-max") == 0)
				current_repo->sync_openpgp_key_refresh_retry_delay_max = (int)strtol(value, NULL, 10);
			else if (strcmp(name, "sync-openpgp-key-refresh-retry-delay-mult") == 0)
				current_repo->sync_openpgp_key_refresh_retry_delay_mult = (int)strtol(value, NULL, 10);
			else if (strcmp(name, "sync-openpgp-key-refresh-retry-overall-timeout") == 0)
				current_repo->sync_openpgp_key_refresh_retry_overall_timeout = (int)strtol(value, NULL, 10);
			else if (strcmp(name, "sync-rcu") == 0)
				current_repo->sync_rcu = (int)strtol(value, NULL, 10);
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
				current_repo->sync_rsync_verify_max_age = (int)strtol(value, NULL, 10);
			else if (strcmp(name, "sync-rsync-verify-jobs") == 0)
				current_repo->sync_rsync_verify_jobs = (int)strtol(value, NULL, 10);
			else if (strcmp(name, "sync-rsync-extra-opts") == 0) {
				n = 0;
				current_repo->sync_rsync_extra_opts= strsplt(value, &n);
				current_repo->sync_rsync_extra_opts[n] = NULL;
			}
			else if (strcmp(name, "sync-rsync-verify-metamanifest") == 0)
				current_repo->sync_rsync_verify_metamanifest = (int)strtol(value, NULL, 10);
			else if (strcmp(name, "auto_sync") == 0) {
				if (strcmp(value, "true") == 0 || strcmp(value, "yes") == 0)
					current_repo->auto_sync = 1;
				else
					current_repo->auto_sync = 0;
			}
		}
		free(line);
	}
	
	fpnode_free(repos_conf);
	
	return current_repo;
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
		
		if (curr->categories) {
			for (i = 0; i < curr->categories->n; i++) {
				StringVector* cat = (StringVector*)vector_get(curr->categories, i);
				string_vector_free(cat);
			}
			vector_free(curr->categories);
		}
		
		if (curr->packages)
			map_free(curr->packages, (void (*) (void*))package_free);
		
		free(curr);
		curr = next;
	}
	
}

StringVector* prv_repository_category_init(Repository* repo, char* category) {
	char* manifest_path;
	asprintf(&manifest_path, "%s/%s/Manifest.gz", repo->location, category);
	FILE* fp = fread_archive(manifest_path);
	
	if (!fp) {
		plog_error("Failed to open category manifest %s", manifest_path);
		free(manifest_path);
		return NULL;
	}
	
	free(manifest_path);
	
	char* line = NULL;
	size_t line_size = 0;
	
	StringVector* out = string_vector_new();
	
	while (getline(&line, &line_size, fp) > 0) {
		char* data_type = strtok(line, " ");
		if (strcmp(data_type, "MANIFEST") != 0)
			continue;
		
		char* package_name = strtok(NULL, "/");
		string_vector_add(out, package_name);
	}
	
	free(line);
	fclose(fp);
	
	return out;
}

int repository_init(Repository* repo) {
	char* repo_manifest_path = NULL;
	asprintf(&repo_manifest_path, "%s/profiles/categories", repo->location);
	
	FILE* cat_manifest = fopen(repo_manifest_path, "r");
	free(repo_manifest_path);
	
	if (!cat_manifest) {
		plog_error("Invalid repository location %s", repo->location);
		return 0;
	}
	
	char* category = NULL;
	size_t line_size = 0;
	
	ssize_t line_len = 0;
	while ((line_len = getline(&category, &line_size, cat_manifest)) > 0) {
		category[line_len - 1] = 0; // Remove new line
		
		StringVector* cat = prv_repository_category_init(repo, category);
		if (!cat)
			return 0;
		
		string_vector_add(repo->categories_names, category);
		vector_add(repo->categories, cat);
	}
	
	free(category);
	fclose(cat_manifest);
	
	CacheHandler* cache_handler = cache_handler_new(repo->parent->jobs);
	
	for (int i = 0; i < repo->categories->n; i++) {
		char* category_name = string_vector_get(repo->categories_names, i);
		StringVector* category_packages = (StringVector*)vector_get(repo->categories, i);
		
		for (int j = 0; j < category_packages->n; j++) {
			char* pm_path = NULL;
			char* package_name = string_vector_get(category_packages, j);
			asprintf(&pm_path, "%s/%s/%s/Manifest", repo->location, category_name, package_name);
			
			FILE* package_manifest = fopen(pm_path, "r");
			if (!package_manifest) {
				plog_error("Failed to open package manifest %s/%s", category_name, package_name);
				return 0;
			}
			
			char* line = NULL;
			line_size = 0;
			
			while (getline(&line, &line_size, package_manifest) > 0) {
				char* type = strtok(line, " ");
				if (strcmp(type, "EBUILD") != 0)
					continue;
				
				char* ebuild_file = strtok(NULL, " \n");
				char* pf_end = strstr(ebuild_file, ".ebuild");
				*pf_end = 0;
				
				Ebuild* current = package_init(repo, category_name, ebuild_file);
				
				if (repo->parent->options & EMERGE_CACHE) {
					if (cache_verify(current->cache_file, current->ebuild_md5))
						continue;
					cache_handler_request(cache_handler, current);
				}
			}
			
			free(line);
			fclose(package_manifest);
		}
	}
	
	cache_handler_finish(cache_handler);
	
	return 1;
}