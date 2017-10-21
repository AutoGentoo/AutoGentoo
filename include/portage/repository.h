//
// Created by atuser on 10/20/17.
//

#include <tools/vector.h>
#include <stdbool.h>
#include <unitypes.h>
#include <sys/stat.h>
#include <stdio.h>
#include <tools/string_vector.h>

#ifndef HACKSAW_REPOSITORY_H
#define HACKSAW_REPOSITORY_H

typedef struct __RepoConfig RepoConfig;
typedef struct __Repository Repository;
typedef enum __repo_t repo_t;

enum __repo_t {
    CVS,
    GIT,
    RSYNC,
    SVN,
    WEBRSYNC
};

struct __RepoConfig {
    Repository* main_repo;
    bool eclass_overides;
    StringVector* force; // Essentially just char[][]
    Vector* repositories;

};

struct __Repository {
    char name[32];

    /* Disabled features
    char* aliases;
    Vector* masters;
    int clone-depth;
    char* sync_user;
    */

    bool auto_sync;
    bool eclass_overrides; // Override the eclass definition (NOT RECOMMENDED)
    StringVector* force;

    char location[256];
    int priority;
    char sync_cvs_repo[256];
    repo_t sync_type;
    mode_t sync_mask;
    char sync_uri[256]; // Empty to disable syncing
};

void parse_repo_config (RepoConfig* repo_config, FILE* fp);
void parse_repository (Repository* repo, FILE* fp);
repo_t get_sync_type (char* str);

#endif //HACKSAW_REPOSITORY_H
