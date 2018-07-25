//
// Created by atuser on 10/21/17.
//

#include <string.h>
#include <stdlib.h>
#include <autogentoo/hacksaw/portage/repository.h>

char* sync_types[] = {
		"cvs",
		"git",
		"rsync",
		"svn",
		"webrsync"
};

RepoConfig* repo_config_new() {
	RepoConfig* repo_conf = malloc(sizeof(RepoConfig));
	
	repo_conf->config = vector_new(sizeof(Conf*), REMOVE | UNORDERED);
	repo_conf->repositories = vector_new(sizeof(Repository*), REMOVE | UNORDERED);
	repo_conf->eclass_overrides = NULL;
	repo_conf->force = NULL;
	
	return repo_conf;
}

void repo_config_read(RepoConfig* repo_conf, char* filepath) {
	Conf* conf = conf_new(filepath);
	vector_add(repo_conf->config, &conf);
	if (repo_conf->eclass_overrides != NULL) {
		string_vector_free(repo_conf->eclass_overrides);
	}
	if (repo_conf->force != NULL) {
		string_vector_free(repo_conf->force);
	}
	repo_conf->eclass_overrides = conf_get_vector(conf, "DEFAULT", "eclass-overrides");
	repo_conf->force = conf_get_vector(conf, "DEFAULT", "force");
	
	int i;
	for (i = 0; i != conf->sections->n; i++) {
		char* current_section = string_vector_get(conf->sections, i);
		if (strcmp(current_section, "DEFAULT") == 0)
			continue;
		Repository* temp = parse_repository(conf, current_section);
		vector_add(repo_conf->repositories, &temp);
	}
}

Repository* parse_repository(Conf* conf, char *section) {
	Repository* repo = malloc(sizeof(Repository));
	strcpy(repo->name, section);
	
	repo->eclass_overrides = conf_get_vector(conf, section, "eclass-overrides");
	repo->force = conf_get_vector(conf, section, "force");
	conf_get_convert(conf, (char*) repo->location, section, "location");
	conf_get_convert(conf, (char*) repo->sync_cvs_repo, section, "sync-cvs-repo");
	conf_get_convert(conf, (char*) repo->sync_uri, section, "sync-uri");
	char sync_type_buff[32];
	int has = conf_get_convert(conf, (char*) sync_type_buff, section, "sync-type");
	if (has) {
		if ((repo->sync_type = (repo_t) string_find(sync_types, sync_type_buff, 5)) == -1) {
			lerror("error in file %s", conf->path);
			lerror("sync type '%s' is invalid", sync_type_buff);
			exit(1);
		}
	}
	
	char auto_sync_buff[16];
	if (conf_get_convert(conf, (char*) auto_sync_buff, section, "auto-sync")) {
		if (strcmp("yes", auto_sync_buff) == 0) {
			repo->auto_sync = 0;
		} else if (strcmp("no", auto_sync_buff) == 0) {
			repo->auto_sync = 1;
		} else {
			lwarning("invalid token: '%s' (%s)", auto_sync_buff, conf->path);
		}
	}
	
	char priority_buff[8];
	if (conf_get_convert(conf, (char*) priority_buff, section, "priority")) {
		repo->priority = (int) strtol(priority_buff, NULL, 10);
	}
	
	char cat_file[256];
	sprintf(cat_file, "/%s/profiles/categories", repo->location);
	fix_path(cat_file);
	repo->packages = map_new(1024, 128);
	
	FILE* fp = fopen(cat_file, "r");
	char* line;
	size_t len = 0;
	ssize_t read;
	
	if (fp == NULL) {
		lerror("no such file or directory: %s", cat_file);
		exit(1);
	}
	
	while ((read = getline(&line, &len, fp)) != -1) {
		line[strlen(line) - 1] = 0; // Remove the newline
		category_read(repo, line);
	}
	
	fclose(fp);
	return repo;
}

void repo_config_free(RepoConfig* ptr) {
	int i;
	for (i = 0; i != ptr->config->n; i++) {
		conf_free(*(Conf**) vector_get(ptr->config, i));
	}
	for (i = 0; i != ptr->repositories->n; i++) {
		repository_free(*(Repository**) vector_get(ptr->repositories, i));
	}
	
	string_vector_free(ptr->force);
	string_vector_free(ptr->eclass_overrides);
	free(ptr);
}

void repository_free(Repository* ptr) {
	if (ptr->force != NULL) {
		string_vector_free(ptr->force);
	}
	if (ptr->eclass_overrides != NULL) {
		string_vector_free(ptr->eclass_overrides);
	}
	free(ptr);
}