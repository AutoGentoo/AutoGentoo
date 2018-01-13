//
// Created by atuser on 10/23/17.
//

#ifndef HACKSAW_DIRECTORY_H
#define HACKSAW_DIRECTORY_H

#include "portage.h"

typedef struct __PortageDirectory PortageDirectory;

typedef enum {
    P_DIRECTORY,
    P_FILE,
    P_NOT_FOUND
} path_t;

typedef enum {
    PD_CLOSED,
    PD_OPEN
} portdir_t;

struct __PortageDirectory {
    Portage* parent;
    char* dir_name;
    portdir_t status;
    Vector* fp_list;    // List of FILE* structs, 
                        //if directory is a file then length will be 1
};

StringVector* get_directories (char* path);
StringVector* get_files (char* path);
path_t check_is_dir (char* path);
PortageDirectory* portage_directory_read (Portage* portage, char* location);
void portage_directory_close (PortageDirectory* pdir);
void portage_directory_free (PortageDirectory* pdir);

#endif //HACKSAW_DIRECTORY_H
