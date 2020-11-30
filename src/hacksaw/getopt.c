//
// Created by atuser on 1/13/18.
//

#include <string.h>
#include <stdlib.h>
#include "getopt.h"
#include "log.h"
#include "getopt.h"

char** opt_handle(Opt* opts, int argc, char** argv) {
    int i;
    char** out = calloc(argc, sizeof(char*));
    int out_i = 0;

    for (i = 0; i != argc; i++) {
        if (argv[i] == NULL)
            continue;

        if (argv[i][0] != '-') {
            out[out_i++] = strdup(argv[i]);
            continue;
        }

        Opt* op = find_opt(opts, argv[i]);
        if (op == NULL) {
            lerror("option '%s' not found!", argv[i]);
            exit(1);
        }

        if (op->opt & OPT_ARG) {
            i++;
            if (i == argc) {
                lerror("option '%s' requires an argument!", argv[i]);
                exit(1);
            }
            op->handler(op, argv[i]);
        } else
            op->handler(op, NULL);
    }

    return out;
}

Opt* find_opt(Opt* opts, char* arg) {
    opt_opts_t t = get_type(arg);
    if (t == OPT_ARG)
        return NULL;

    char _short_buf[2];
    Opt* current = &opts[0];
    while (current->handler != NULL) {
        if (t == OPT_SHORT && current->opt & OPT_SHORT) {
            _short_buf[0] = current->_short;
            _short_buf[1] = 0;

            if (strcmp(_short_buf, (arg + 1)) == 0)
                return current;
        } else if (t == OPT_LONG && current->opt & OPT_LONG) {
            if (strcmp(current->_long, arg + 2) == 0)
                return current;
        }
        current++;
    }

    return NULL;
}

opt_opts_t get_type(char* arg) {
    if (*arg == '-') {
        if (*(arg + 1) == '-')
            return OPT_LONG;
        return OPT_SHORT;
    }

    return OPT_ARG;
}

void print_help(Opt* opts) {
    printf("Options:\n");

    Opt* current = &opts[0];
    while (current->handler != NULL) {
        printf("  ");
        int psize = 0;
        if (current->opt & OPT_SHORT) {
            printf("-%c ", current->_short);
            psize += 3;
        }
        if (current->opt & OPT_LONG) {
            printf("--%s", current->_long);
            psize += 2 + strlen(current->_long);
        }

        if (current->opt & OPT_ARG) {
            printf(" =<arg>");
            psize += 7;
        }

        printf("%*s%s\n", 30 - psize, " ", current->help);

        current++;
    }

    printf("\n");
}