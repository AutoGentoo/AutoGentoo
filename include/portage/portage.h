//
// Created by atuser on 11/22/17.
//

#ifndef HACKSAW_PORTAGE_H
#define HACKSAW_PORTAGE_H

#include <portage/repository.h>

typedef struct __Portage Portage;

struct __Portage {
    char* root_dir;
    RepoConfig* repos;
    
};

#endif
