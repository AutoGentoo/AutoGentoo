//
// Created by atuser on 10/21/17.
//

#include <portage/repository.h>
#include <string.h>
#include <tools/regular_expression.h>

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
    int i;
    for (i=0; i!=sections->n; i++, current_section = string_vector_get(sections, i)) {
        if (strcmp (current_section, "DEFAULT") == 0) {
            StringVector* variable_parsed;
            re_group_match(variable_parsed, line, "(.*) = (.*)", 2);
            if (strcmp(string_vector_get(variable_parsed, 0), "eclass-overides") == 0) {
                string_vector_split(repo_config->eclass_overides, string_vector_get(variable_parsed, 1), " "); // Split by " "
            }
            else if (strcmp (string_vector_get(variable_parsed, 1), "force") == 0) {
                string_vector_split(repo_config->force, string_vector_get(variable_parsed, 1), " "); // Split by " "
            }
            else if (strcmp ())
        }
    }
}
void parse_repository (Repository* repo, FILE* fp);
repo_t get_sync_type (char* str);