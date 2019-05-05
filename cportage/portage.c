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
		plog_warn("Could not get hash - %s [%d]", strerror(errno));
		return -1;
	}
	
	int fd = open(path, O_RDONLY);
	return portage_get_hash_fd(target, fd, algorithm);
}

Repository* repository_new (char* name, char* root, char* conf_dir) {
	Repository* out = malloc(sizeof(Repository));
	
	out->packages = map_new(32768, 0.8);
	out->name = strdup(name);
	out->root_path = strdup(root);
	out->conf_dir = strdup(conf_dir);
	
	char* package_accept_keywords_path;
	char* package_use_path;
	char* make_conf_path;
	asprintf(&package_accept_keywords_path, "%s/%s/package.accept_keywords", root, conf_dir);
	asprintf(&package_use_path, "%s/%s/package.use", root, conf_dir);
	asprintf(&make_conf_path, "%s/%s/make.conf", root, conf_dir);
	
	portage_get_hash(&out->package_accept_keywords, package_accept_keywords_path, EVP_sha256());
	portage_get_hash(&out->package_use, package_use_path, EVP_sha256());
	portage_get_hash(&out->make_conf, make_conf_path, EVP_sha256());
	
	free(package_accept_keywords_path);
	free(package_use_path);
	free(make_conf_path);
	
	return out;
}