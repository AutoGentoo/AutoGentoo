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

void parse_repo_config (RepoConfig* repo_config) {
    char* line;
    size_t len = 0;
    ssize_t read;

    repo_config->repositories = vector_new(sizeof(Repository*), REMOVE | UNORDERED); // Repository too large to malloc so many at a time
    repo_config->force = string_vector_new();
    repo_config->eclass_overides = string_vector_new();
}

Repository* parse_repository (ConfigSection* section) {
    Repository* repo = malloc (sizeof(Repository));

    repo->eclass_overrides = config_get_vector(section->parent, section->name, "eclass-overrides");
    repo->force = config_get_vector(section->parent, section->name, "force");
    config_get_convert(section->parent, (char**)&repo->location, section->name, "location");
    config_get_convert(section->parent, (char**)&repo->sync_cvs_repo, section->name, "sync-cvs-repo");
    config_get_convert(section->parent, (char**)&repo->sync_uri, section->name, "sync-uri");
    char sync_type_buff[32];
    int has = config_get_convert(section->parent, (char**)&sync_type_buff, section->name, "sync-type");
    if (has) {
        if ((repo->sync_type = (repo_t)string_find(sync_types, sync_type_buff, 5)) == -1) {
            lerror("error in file %s", section->parent->path);
            lerror("sync type '%s' is invalid", sync_type_buff);
            exit(1);
        }
    }

    char auto_sync_buff[16];
    if (config_get_convert(section->parent, (char**)&auto_sync_buff, section->name, "auto-sync")) {
        if (strcmp ("yes", auto_sync_buff) == 0) {
            repo->auto_sync = true;
        }
        else if(strcmp ("yes", auto_sync_buff) == 0) {
            repo->auto_sync = false;
        }
        else {
            lwarning("invalid token: '%s' (%s)", auto_sync_buff, section->parent->path);
        }
    }

    char priority_buff[8];
    if (config_get_convert(section->parent, (char**)&priority_buff, section->name, "priority")) {
        repo->priority = (int)strtol(priority_buff, NULL, 10);
    }

    return repo;
}

repo_t get_sync_type (char* str) {

}