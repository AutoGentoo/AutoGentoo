//
// Created by atuser on 10/20/17.
//

#include <tools/vector.h>
#include <unitypes.h>
#include <sys/stat.h>
#include <stdio.h>
#include <tools/conf.h>
#include <tools/map.h>
#include <portage/package.h>

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
    StringVector* eclass_overrides;
    StringVector* force;
    Vector* repositories;
    Vector* config;
};

struct __Repository {
    char name[32];

    /* Disabled features
    char* aliases;
    Vector* masters;
    int clone-depth;
    char* sync_user;
    mode_t sync_mask;
    */

    int auto_sync;
    StringVector* eclass_overrides; // Override the eclass definition (NOT RECOMMENDED)
    StringVector* force;

    char location[256];
    int priority;
    char sync_cvs_repo[256];
    repo_t sync_type;
    char sync_uri[256]; // Empty to disable syncing
    
    /*
     *   cat/pkgname : Package*
     */
    Map* packages;
};

RepoConfig* repo_config_new ();
void repo_config_read (RepoConfig* repo_config, char* filepath);
Repository* parse_repository (ConfSection*);
void repository_sync (Repository* repo);
void repo_config_free(RepoConfig* ptr);
void repository_free (Repository* ptr);

#endif //HACKSAW_REPOSITORY_H
