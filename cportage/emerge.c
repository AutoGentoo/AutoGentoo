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
#include "suggestion.h"
#include <unistd.h>

int number_len(int num) {
	char out[32];
	sprintf(out, "%d", num);
	return (int)strlen(out);
}

Emerge* emerge_new() {
	Emerge* out = malloc(sizeof(Emerge));
	
	out->target_arch = ARCH_AMD64;
	
	out->atoms = NULL;
	out->buildroot = strdup("/");
	out->installroot = strdup("/");
	out->root = strdup("/");
	out->options = 0;
	out->repos = NULL;
	out->default_repo_name = NULL;
	out->default_repo = NULL;
	
	out->use_suggestions = NULL;
	
	
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
	asprintf(&metadata_path, "%s/metadata/md5-cache", emerge->default_repo->location);
	emerge->default_repo->category_manifests = manifest_metadata_parse(metadata_path);
	free(metadata_path);
	if (!emerge->default_repo->category_manifests) {
		return errno;
	}
	
	manifest_metadata_deep(emerge->default_repo->category_manifests);
	
	Manifest* current_cat;
	Manifest* current_pkg;
	for (int i = 0; i < emerge->default_repo->category_manifests->n; i++) {
		current_cat = (Manifest*)vector_get(emerge->default_repo->category_manifests, i);
		for (int j = 0; j < current_cat->parsed->n; j++) {
			current_pkg = (Manifest*)vector_get(current_cat->parsed, j);
			package_init(emerge->default_repo, current_cat, current_pkg);
		}
	}
	
	/* Initialize the profile */
	emerge->profile = profile_new();
	
	char profile_realpath[1024];
	size_t profile_len = 0;
	if ((profile_len = readlink("/etc/portage/make.profile", profile_realpath, sizeof(profile_realpath)-1)) == -1) {
		portage_die("Failed to read make.profile link");
	}
	
	profile_realpath[profile_len] = 0;
	profile_parse(emerge->profile, "", profile_realpath);
	
	/* Do this before package.use because globals need to be applied to packages on metadata_init()  */
	emerge->use_expand = use_expand_new(emerge->default_repo);
	make_conf_parse(emerge);
	make_conf_use(emerge);
	
	emerge_parse_keywords(emerge);
	emerge_parse_useflags(emerge);
	
	char* dep_expr_buff = NULL;
	size_t dep_expr_size = 0;
	for (int i = 0; emerge->atoms[i]; i++)
		dep_expr_size += strlen(emerge->atoms[i]) + 1;
	
	dep_expr_buff = malloc(dep_expr_size);
	dep_expr_buff[0] = 0;
	
	for (int i = 0; emerge->atoms[i]; i++) {
		strcat(dep_expr_buff, emerge->atoms[i]);
		strcat(dep_expr_buff, " ");
	}
	
	printf("---------\n");
	
	Dependency* dep = cmdline_parse(dep_expr_buff);
	if (!dep)
		portage_die("Failed to parse arguments");
	free(dep_expr_buff);
	
	for (Dependency* expand_dep = dep; expand_dep; expand_dep = expand_dep->next) {
		if (expand_dep->atom && strcmp(expand_dep->atom->category, "SEARCH") == 0) {
			StringVector* valid_categories = string_vector_new();
			for (int i = 0; i < emerge->default_repo->category_manifests->n; i++) {
				Manifest* current_cat_search = vector_get(emerge->default_repo->category_manifests, i);
				
				char* cat_splt = strchr(current_cat_search->filename, '/');
				*cat_splt = 0;
				
				free(expand_dep->atom->key);
				asprintf(&expand_dep->atom->key, "%s/%s", current_cat_search->filename, expand_dep->atom->name);
				
				Package* try_package = atom_resolve_package(emerge, expand_dep->atom);
				if (try_package)
					string_vector_add(valid_categories, expand_dep->atom->key);
				
				*cat_splt = '/';
			}
			
			if (valid_categories->n == 0)
				portage_die("Could not resolve package name %s", expand_dep->atom->name);
			else if (valid_categories->n > 1) {
				errno = 0;
				plog_error("Ambigious name %s", expand_dep->atom->name);
				printf("Found the following packages:\n");
				for (int i = 0; i < valid_categories->n; i++)
					printf("  -  %s\n", string_vector_get(valid_categories, i));
				portage_die("Pass the full package qualifier");
			}
			else
				expand_dep->atom->key = string_vector_get(valid_categories, 0);
			
			vector_free(valid_categories);
		}
	}
	
	Vector* selected = pd_layer_resolve(emerge, dep);
	Suggestion* current = NULL;
	
	/* The resolution failed, try again with these suggestion settings */
	int i = 0;
	while (emerge->use_suggestions) {
		emerge_apply_suggestions(emerge); // Updates package.use
		emerge_apply_package_use(emerge); // Updates IUSE in ebuilds
		
		current = emerge->use_suggestions;
		emerge->use_suggestions = NULL;
		printf("%d\n", ++i);
		
		pd_layer_resolve(emerge, dep);    // Resolve again
	}
	
	int max_width = number_len(selected->n);
	for (int i = 0; i < selected->n; i++) {
		SelectedEbuild* eb = vector_get(selected, i);
		printf("(%*d) ", max_width, i + 1);
		
		selected_ebuild_print(emerge, eb);
	}
	
	for (int i = 0; i < selected->n; i++) {
		selected_ebuild_free(vector_get(selected, i));
	}
	
	vector_free(selected);
	
	return 0;
}

void emerge_free(Emerge* em) {
	map_free(em->use_expand, NULL);
	
	free(em->buildroot);
	free(em->installroot);
	free(em->root);
	
	repository_free(em->repos);
	
	free(em);
}