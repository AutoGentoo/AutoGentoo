//
// Created by atuser on 10/18/17.
//

#include <stdio.h>
#include <tools/vector.h>
#include <tools/string_vector.h>
#include <config/conf.h>
#include <test/debug.h>
#include <portage/repository.h>
#include <portage/manifest.h>

int main() {
    RepoConfig* repoconf = repo_config_new();
    repo_config_read(repoconf, "/etc/portage/repos.conf");
    repo_config_free(repoconf);
    return 0;
}
