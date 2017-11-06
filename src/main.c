//
// Created by atuser on 10/18/17.
//

#include <Hacksaw.h>

int main(int argc, char* argv[]) {
    /*RepoConfig* repoconf = repo_config_new();
    repo_config_read(repoconf, "/etc/portage/repos.conf");
    repo_config_free(repoconf);
    */
    print_package_selector(get_selector_from_depend (">=gnome-base/gnome-3.22.0-r4:3[gtk, wayland?]"));
    return 0;
}
