//
// Created by atuser on 10/18/17.
//

#include <stdio.h>
#include <portage/repository.h>
#include <stdlib.h>
#include <string.h>
#include <tools/map.h>

int main() {
    RepoConfig* repoconf = repo_config_new();
    repo_config_read(repoconf, "/etc/portage/repos.conf");
    repo_config_free(repoconf);
    return 0;
}
