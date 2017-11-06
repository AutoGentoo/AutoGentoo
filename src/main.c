//
// Created by atuser on 10/18/17.
//

#include <Hacksaw.h>

int main(int argc, char* argv[]) {
    RepoConfig* repoconf = repo_config_new();
    repo_config_read(repoconf, "/etc/portage/repos.conf");
    repo_config_free(repoconf);
    return 0;
}
