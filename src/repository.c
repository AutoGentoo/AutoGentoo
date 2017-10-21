//
// Created by atuser on 10/21/17.
//

#include <portage/repository.h>
#include <string.h>

void parse_repo_config (RepoConfig* repo_config, FILE* fp) {
    char* line;
    size_t len = 0;
    ssize_t read;

    repo_config->repositories = vector_new(sizeof(Repository), REMOVE | UNORDERED);
    while ((read = getline(&line, &len, fp))) {
        line[strlen(line) - 1] = '\0'; // Remove the newline
        //if ()
    }
}
void parse_repository (Repository* repo, FILE* fp);
repo_t get_sync_type (char* str);