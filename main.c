#include <stdio.h>
#include <tools/vector.h>
#include <tools/string_vector.h>
#include <tools/regular_expression.h>
#include <config/config.h>
#include <test/debug.h>
#include <portage/repository.h>

void print_bin (void* ptr, int n, size_t size) {
    int i;
    for (i=0; i != (size * n); i++) {
        int j;
        for (j=0; j<8; j++) {
            printf("%d",  !!((((char*)ptr)[i] << j) & 0x80));
        }

        if (!((i + 1) % size)) {
            printf (" ");
        }
    }
    printf("\n");
}

void print_vec(Vector* vec) {
    print_bin(vec->ptr, vec->n, vec->size);
    fflush(stdout);
}

void print_string_vec(StringVector* vec) {
    int i;
    for (i=0; i!=vec->n; i++) {
        printf("%p (%s) ", string_vector_get(vec, i), string_vector_get(vec, i));
    }
    printf("\n");
}

void print_config_variable (ConfigVariable* var) {
    printf ("%s = %s\n", var->identifier, var->value);
}

void print_config (Config* config) {
    int i;
    for (i=0; i!=config->default_variables->n; i++) {
        print_config_variable(vector_get(config->default_variables, i));
    }
    for (i=0; i!=config->sections->n; i++) {
        ConfigSection* current_section = *(ConfigSection**)vector_get(config->sections, i);
        printf("[%s]\n", current_section->name);
        int j;
        for (j=0; j!=current_section->variables->n; j++) {
            print_config_variable(vector_get(current_section->variables, j));
        }
    }
}

struct __test1 {
    int a;
    int b;
};

struct __test2 {
    int a;
    int b;
    int c;
};

int main() {
    Config* test_config = config_new("/etc/portage/make.conf");
    printf("CFLAGS = %s\n", config_get(test_config, NULL, "CFLAGS"));
    printf("PORTDIR = %s\n", config_get(test_config, NULL, "PORTDIR"));
    config_free(test_config);

    Config* test_repo = config_new("/etc/portage/repos.conf/rrr");
    printf("DEFAULT.main-repo = %s\n", config_get(test_repo, "DEFAULT", "main-repo"));
    //print_config(test_repo);
    config_free(test_repo);

    RepoConfig* configrepo = repo_config_new();
    repo_config_read (configrepo, "/etc/portage/repos.conf/rrr");
    printf ("repo1->name: %s\n", (*(Repository**)vector_get(configrepo->repositories, 0))->name);
    printf ("repo1->sync-uri: %s\n", (*(Repository**)vector_get(configrepo->repositories, 0))->sync_uri);
    repo_config_free(configrepo);

    return 0;
}