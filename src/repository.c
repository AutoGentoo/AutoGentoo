//
// Created by atuser on 10/21/17.
//

#include <portage/repository.h>
#include <string.h>
#include <tools/regular_expression.h>
#include <stdlib.h>
#include <tools/string.h>
#include <tools/log.h>

char* sync_types[] = {
        "cvs",
        "git",
        "rsync",
        "svn",
        "webrsync"
};

RepoConfig* repo_config_new () {
    RepoConfig* repo_config = malloc(sizeof(RepoConfig));

    repo_config->config = vector_new(sizeof(Config*), REMOVE | UNORDERED);
    repo_config->repositories = vector_new(sizeof(Repository*), REMOVE | UNORDERED);
    repo_config->eclass_overrides = NULL;
    repo_config->force = NULL;
}

void repo_config_read (RepoConfig* repo_config, char* filepath) {
    Config* config = config_new(filepath);
    vector_add(repo_config->config, &config);
    if (repo_config->eclass_overrides != NULL) {
        string_vector_free(repo_config->eclass_overrides);
    }
    if (repo_config->force != NULL) {
        string_vector_free(repo_config->force);
    }
    repo_config->eclass_overrides = config_get_vector(config, "DEFAULT", "eclass-overrides");
    repo_config->force = config_get_vector(config, "DEFAULT", "force");

    int i;
    for (i = 0; i != config->sections->n; i++) {
        ConfigSection* current_section = *(ConfigSection**)vector_get(config->sections, i);
        if (strcmp(current_section->name, "DEFAULT") == 0) {
            continue;
        }
        Repository* temp = parse_repository(current_section);
        vector_add(repo_config->repositories, &temp);
    }
}

Repository* parse_repository (ConfigSection* section) {
    Repository* repo = malloc (sizeof(Repository));
    strcpy(repo->name, section->name);

    repo->eclass_overrides = config_get_vector(section->parent, section->name, "eclass-overrides");
    repo->force = config_get_vector(section->parent, section->name, "force");
    config_get_convert(section->parent, (char*)repo->location, section->name, "location");
    config_get_convert(section->parent, (char*)repo->sync_cvs_repo, section->name, "sync-cvs-repo");
    config_get_convert(section->parent, (char*)repo->sync_uri, section->name, "sync-uri");
    char sync_type_buff[32];
    int has = config_get_convert(section->parent, (char*)sync_type_buff, section->name, "sync-type");
    if (has) {
        if ((repo->sync_type = (repo_t)string_find(sync_types, sync_type_buff, 5)) == -1) {
            lerror("error in file %s", section->parent->path);
            lerror("sync type '%s' is invalid", sync_type_buff);
            exit(1);
        }
    }

    char auto_sync_buff[16];
    if (config_get_convert(section->parent, (char*)auto_sync_buff, section->name, "auto-sync")) {
        if (strcmp ("yes", auto_sync_buff) == 0) {
            repo->auto_sync = true;
        }
        else if(strcmp ("no", auto_sync_buff) == 0) {
            repo->auto_sync = false;
        }
        else {
            lwarning("invalid token: '%s' (%s)", auto_sync_buff, section->parent->path);
        }
    }

    char priority_buff[8];
    if (config_get_convert(section->parent, (char*)priority_buff, section->name, "priority")) {
        repo->priority = (int)strtol(priority_buff, NULL, 10);
    }

    return repo;
}

void repo_config_free(RepoConfig* ptr) {
    int i;
    for (i=0; i != ptr->config->n; i++) {
        config_free (*(Config**)vector_get(ptr->config, i));
    }
    for (i=0; i != ptr->repositories->n; i++) {
        repository_free (*(Repository**)vector_get(ptr->repositories, i));
    }

    string_vector_free(ptr->force);
    string_vector_free(ptr->eclass_overrides);
    free (ptr);
}

void repository_free (Repository* ptr) {
    if (ptr->force != NULL) {
        string_vector_free(ptr->force);
    }
    if (ptr->eclass_overrides != NULL) {
        string_vector_free(ptr->eclass_overrides);
    }
    free(ptr);
}