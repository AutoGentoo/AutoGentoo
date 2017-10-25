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

    return 0;
}
