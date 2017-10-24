//
// Created by atuser on 10/18/17.
//

#include <stdio.h>
#include <tools/vector.h>
#include <tools/string_vector.h>
#include <config/conf.h>
#include <test/debug.h>
#include <portage/repository.h>

int main() {
    Conf* test_config = conf_new("/etc/portage/make.conf");
    printf("CFLAGS = %s\n", conf_get(test_config, NULL, "CFLAGS"));
    printf("PORTDIR = %s\n", conf_get(test_config, NULL, "PORTDIR"));
    conf_free(test_config);

    Conf* test_repo = conf_new("/etc/portage/repos.conf/rrr");
    printf("DEFAULT.main-repo = %s\n", conf_get(test_repo, "DEFAULT", "main-repo"));
    //print_config(test_repo);
    conf_free(test_repo);

    RepoConfig* configrepo = repo_config_new();
    repo_config_read (configrepo, "/etc/portage/repos.conf/rrr");
    printf ("repo1->name: %s\n", (*(Repository**)vector_get(configrepo->repositories, 0))->name);
    printf ("repo1->sync-uri: %s\n", (*(Repository**)vector_get(configrepo->repositories, 0))->sync_uri);
    repo_config_free(configrepo);

    return 0;
}
