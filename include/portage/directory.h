//
// Created by atuser on 10/23/17.
//

#ifndef HACKSAW_DIRECTORY_H
#define HACKSAW_DIRECTORY_H

#include <tools/string_vector.h>

typedef struct __PortageDirectory PortageDirectory;

struct __PortageDirectory {
    char* path;
    //char* 
};

StringVector* get_directories (char* path);

#endif //HACKSAW_DIRECTORY_H
