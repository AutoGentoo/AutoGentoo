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
#include "dependency.h"
#include "globals.h"

Emerge* emerge_new() {
	Emerge* out = malloc(sizeof(Emerge));
	
	out->target_arch = ARCH_AMD64;
	
	out->atoms = NULL;
	out->buildroot = strdup("/");
	out->installroot = strdup("/");
	out->root = strdup("/");
	out->options = 0;
	out->repo = NULL;
	out->default_repo_ptr = NULL;
	out->default_repo = NULL;
	
	out->use_suggestions = NULL;
	out->keyword_suggestions = NULL;
	
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
	asprintf(&metadata_path, "%s/metadata/md5-cache", emerge->default_repo_ptr->location);
	emerge->repo->category_manifests = manifest_metadata_parse(metadata_path);
	free(metadata_path);
	if (!emerge->repo->category_manifests) {
		return errno;
	}
	
	manifest_metadata_deep(emerge->repo->category_manifests);
	
	Manifest* current_cat;
	Manifest* current_pkg;
	for (int i = 0; i < emerge->repo->category_manifests->n; i++) {
		current_cat = (Manifest*)vector_get(emerge->repo->category_manifests, i);
		for (int j = 0; j < current_cat->parsed->n; j++) {
			current_pkg = (Manifest*)vector_get(current_cat->parsed, j);
			package_init(emerge->repo, current_cat, current_pkg);
		}
	}
	/* Do this before package.use because globals need to be applied to packages on metadata_init()  */
	emerge->use_expand = use_expand_new(emerge->default_repo_ptr);
	emerge->make_conf = make_conf_new(emerge);
	emerge->global_use = make_conf_use(emerge);
	
	emerge_parse_keywords(emerge);
	emerge_parse_useflags(emerge);
	
	char* dep_expr_buff = NULL;
	size_t dep_expr_size = 0;
	for (int i = 0; emerge->atoms[i]; i++)
		dep_expr_size += strlen(emerge->atoms[i]) + 1;
	
	dep_expr_buff = malloc(dep_expr_size);
	dep_expr_buff[0] = 0;
	
	for (int i = 0; emerge->atoms[i]; i++)
		strcat(dep_expr_buff, emerge->atoms[i]);
	
	Dependency* dep = depend_parse(dep_expr_buff);
	for (Dependency* check_dep = dep; check_dep; check_dep = check_dep->next) {
		if (check_dep->depends != IS_ATOM) {
			printf("%s\n", check_dep->target);
			dependency_free(dep);
			return 1;
		}
	}
	
	Vector* selected = pd_layer_resolve(emerge, dep);
	
	for (int i = 0; i < selected->n; i++) {
		SelectedEbuild* eb = vector_get(selected, i);
		printf("(%d) ", i);
		selected_ebuild_print(emerge, eb);
	}
	
	return 0;
}
