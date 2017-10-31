#include <stdio.h>
#include <dirent.h>
#include <portage/directory.h>
#include <stdlib.h>

StringVector* get_directories (char* path) {
    StringVector* out = string_vector_new();
    struct dirent *de;
    
    DIR *dr = opendir(path);
 
    if (dr == NULL)  {
        printf("Could not open %s\n", path);
        exit(1);
    }
    
    while ((de = readdir(dr)) != NULL) {
        if (de->d_type == DT_DIR) {
            if (*de->d_name == '.') { // We dont want . and ..
                continue;
            }
            string_vector_add(out, de->d_name);
        }
    }
    closedir(dr);
    return out;
}