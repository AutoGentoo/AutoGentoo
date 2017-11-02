//
// Created by atuser on 10/31/17.
//


#ifndef HACKSAW_USE_H
#define HACKSAW_USE_H

#include <portage/package.h>

typedef struct __UseFlag use_t;
typedef struct __UseDep UseDep;

struct __UseFlag {
    char name[16];
    char enabled;
    char forced;
};

struct __UseDep {
    use_t flag;
    Vector* packages;
};


#endif //HACKSAW_USE_H