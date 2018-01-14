//
// Created by atuser on 1/13/18.
//

#ifndef AUTOGENTOO_GETOPT_H
#define AUTOGENTOO_GETOPT_H

#include <tools/vector.h>

typedef struct _Opt Opt;
typedef void (* AGOPTH) (Opt* op, char* arg);

typedef enum {
    OPT_SHORT = 0x1,
    OPT_LONG = 0x2,
    OPT_ARG = 0x4
} opt_opts_t;

struct _Opt {
    char _short;
    char* _long;
    
    char* help;
    AGOPTH handler;
    opt_opts_t opt;
};

void opt_handle (Opt* opts, int argc, char** argv);
Opt* find_opt (Opt* opts, char* arg);
opt_opts_t get_type (char* arg);
void print_help (Opt* opts);

#endif //AUTOGENTOO_GETOPT_H
