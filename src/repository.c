//
// Created by atuser on 10/21/17.
//

#include <portage/repository.h>
#include <string.h>
#include <tools/regular_expression.h>
#include <stdlib.h>

void parse_repo_config (RepoConfig* repo_config, FILE* fp) {
    char* line;
    size_t len = 0;
    ssize_t read;

    repo_config->repositories = vector_new(sizeof(Repository), REMOVE | UNORDERED);
    repo_config->force = string_vector_new();
    repo_config->eclass_overides = false;

    StringVector* sections = string_vector_new();
    Vector* locations = vector_new(sizeof(fpos_t), ORDERED | KEEP);
    fpos_t position;

    while ((read = getline(&line, &len, fp))) {
        line[strlen(line) - 1] = '\0'; // Remove the newline
        re_group_match(sections, line, "\\[(.*?)\\]", 1);
        fgetpos (fp, &position);
        vector_add (locations, &position);
    }

    char* current_section = NULL;
    char main_repo_name[32];
    int i;
    for (i=0; i!=sections->n; i++, current_section = string_vector_get(sections, i)) {
        if (strcmp (current_section, "DEFAULT") == 0) {
            StringVector* variable_parsed = string_vector_new();
            re_group_match(variable_parsed, line, "(.*) = (.*)", 2);
            char* var_name = string_vector_get(variable_parsed, 0);
            if (strcmp(var_name, "eclass-overides") == 0) {
                string_vector_split(repo_config->eclass_overides, string_vector_get(variable_parsed, 1), " "); // Split by " "
            }
            else if (strcmp (var_name, "force") == 0) {
                string_vector_split(repo_config->force, string_vector_get(variable_parsed, 1), " "); // Split by " "
            }
            else if (strcmp (var_name, "main-repo") == 0) {
                strcpy(main_repo_name, string_vector_get(variable_parsed, 2));
            }
            continue;
        }
        fsetpos(fp, (fpos_t*)vector_get(locations, i));
    }
}

Repository* parse_repository (FILE* fp, size_t stop) {
    Repository* repo = malloc (sizeof(Repository));
    size_t current_read;
    ssize_t read;
    char* line;
    while ((read = getline(&line, &current_read, fp)) &&
            current_read != stop) {

    }
}

repo_t get_sync_type (char* str) {

}