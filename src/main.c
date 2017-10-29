//
// Created by atuser on 10/18/17.
//

#include <stdio.h>
#include <tools/vector.h>
#include <tools/string_vector.h>
#include <config/conf.h>
#include <test/debug.h>
#include <portage/repository.h>
#include <package/manifest.h>

int main() {
    Conf* test_config = conf_new("/etc/portage/make.conf");
    printf("CFLAGS = %s\n", conf_get(test_config, NULL, "CFLAGS"));
    printf("PORTDIR = %s\n", conf_get(test_config, NULL, "PORTDIR"));
    conf_free(test_config);
    
    Manifest man;
    FILE* fp = fopen ("/usr/portage/gnome-base/gnome/Manifest", "r");
    manifest_parse (&man, fp);
    print_bin(&man, 1, sizeof (Manifest));
    fclose (fp);
    
    return 0;
}
