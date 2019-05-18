//
// Created by atuser on 5/9/19.
//

#define _GNU_SOURCE

#include "emerge.h"
#include <string.h>
#include <share.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "portage.h"

Emerge* emerge_new() {
	Emerge* out = malloc(sizeof(Emerge));
	
	out->target_arch = ARCH_AMD64;
	
	out->atoms = NULL;
	out->buildroot = strdup("/");
	out->installroot = strdup("/");
	out->root = strdup("/");
	out->options = 0;
	out->repo = NULL;
	
	return out;
}

int emerge (Emerge* emerge) {
	/*
	char* package_accept_keywords_path;
	char* package_use_path;
	char* make_conf_path;
	asprintf(&package_accept_keywords_path, "%s/etc/portage/package.accept_keywords", emerge->root);
	asprintf(&package_use_path, "%s/etc/portage/package.use", emerge->root);
	asprintf(&make_conf_path, "%s/etc/portage/make.conf", emerge->root);
	
	portage_get_hash(&emerge->hash_accept_keywords, package_accept_keywords_path, EVP_sha256());
	portage_get_hash(&emerge->hash_package_use, package_use_path, EVP_sha256());
	portage_get_hash(&emerge->hash_make_conf, make_conf_path, EVP_sha256());
	
	free(package_accept_keywords_path);
	free(package_use_path);
	free(make_conf_path);
	*/
	
	char* metadata_path = NULL;
	asprintf(&metadata_path, "%s/metadata/md5-cache", "portage_test");
	emerge->repo->category_manifests = manifest_metadata_parse(metadata_path);
	free(metadata_path);
	if (!emerge->repo->category_manifests) {
		return errno;
	}
	
	manifest_metadata_deep(emerge->repo->category_manifests);
	
	Manifest* current_cat;
	Manifest* current_pkg;
	for (current_cat = emerge->repo->category_manifests; current_cat; current_cat = current_cat->next) {
		for(current_pkg = current_cat->parsed; current_pkg; current_pkg = current_pkg->next)
			package_init(emerge->repo, current_cat, current_pkg);
	}
	
	emerge_parse_keywords(emerge);
	
	for (char** atom = emerge->atoms; *atom; atom++) {
	
	}
	
	return 0;
}
