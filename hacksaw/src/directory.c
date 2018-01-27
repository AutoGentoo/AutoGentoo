#include <stdio.h>
#include <dirent.h>
#include <autogentoo/hacksaw/portage/directory.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


StringVector* get_directories (char* path) {
    StringVector* out = string_vector_new();
    struct dirent *de;
    
    DIR *dr = opendir(path);
 
    if (dr == NULL) {
        fprintf(stderr, "Could not open %s\n", path);
        return out;
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

StringVector* get_files (char* path) {
    StringVector* out = string_vector_new();
    
    DIR *dr;
    dr = opendir(path);
    if (dr) {
        struct dirent *de;
        while ((de = readdir(dr)) != NULL) {
            string_vector_add(out, de->d_name);
        }
        closedir(dr);
    }
    return out;
}

path_t check_is_dir (char* path) {
    DIR* directory = opendir(path);

    if(directory != NULL) {
        closedir(directory);
        return P_DIRECTORY;
    }

    if(errno == ENOTDIR) {
        return P_FILE;
    }

    return P_NOT_FOUND;
}


PortageDirectory* portage_directory_read (Portage* portage, char* location) {
    PortageDirectory* out = malloc (sizeof (PortageDirectory));
    out->parent = portage;
    out->status = PD_CLOSED;
    out->dir_name = strdup (location);
    out->fp_list = vector_new (sizeof (FILE*), REMOVE | UNORDERED);
    
    char path[256];
    sprintf (path, "%s/%s", portage->root_dir, location);
    
    if (check_is_dir (path) == P_DIRECTORY) {
        StringVector* files = get_files (path);
        
        int i;
        char _read_path[256];
        for (i = 0; i != files->n; i++) {
            _read_path[0] = 0;
            sprintf (_read_path, "%s/%s", path, string_vector_get (files, i));
            FILE* temp = fopen (_read_path, "r");
            vector_add (out->fp_list, &temp);
        }
        
        string_vector_free(files);
    }
    else {
        FILE* temp = fopen (path, "r");
        vector_add (out->fp_list, &temp);
    }
    
    out->status = PD_OPEN;
    return out;
}

void portage_directory_close (PortageDirectory* pdir) {
    int i;
    for (i = 0; i != pdir->fp_list->n; i++) {
        fclose (*(FILE**)vector_get (pdir->fp_list, i));
    }
    pdir->status = PD_CLOSED;
}

void portage_directory_free (PortageDirectory* pdir) {
    if (pdir->status == PD_OPEN) {
        portage_directory_close(pdir);
    }
    
    free (pdir->dir_name);
    free (pdir);
}