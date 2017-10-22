//
// Created by atuser on 10/21/17.
//

#include <portage/repository.h>
#include <string.h>
#include <tools/regular_expression.h>
#include <stdlib.h>

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

}

repo_t get_sync_type (char* str) {

}