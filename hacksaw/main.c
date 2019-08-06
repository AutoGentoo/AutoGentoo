//
// Created by atuser on 10/18/17.
//

#include "hacksaw.h"

int main(int argc, char* argv[]) {
	RepoConfig* repoconf = repo_config_new();
	repo_config_read(repoconf, "/etc/portage/repos.conf");
	
	print_package_selector(get_selector_from_depend("!>=gnome-base/gnome-3.22.0_alpha-r4:3/3=[gtk, wayland?,-sql(-)]"));
	SmallMap* smap = small_map_new(8, 2);
	small_map_insert(smap, "myconfig", &repoconf);
	printf("%p == %p\n", *(RepoConfig**) small_map_get(smap, "myconfig"), repoconf);
	small_map_free(smap, 0);
	repo_config_free(repoconf);
	return 0;
}
