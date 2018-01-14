#include <stdio.h>
#include <tools/util.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <tools.h>

void prv_mkdir (const char* dir) {
    DIR* _dir = opendir(dir);
    if (_dir) {
        closedir(_dir);
        return;
    }
    char tmp[256];
    char* p = NULL;
    size_t len;
    
    snprintf (tmp, sizeof (tmp), "%s", dir);
    len = strlen (tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            mkdir (tmp, 0777);
            *p = '/';
        }
    }
    mkdir (tmp, 0777);
}

void file_copy (char* src, char* dest) {
    FILE* fp_src = fopen (src, "r");
    if (fp_src == NULL)
        return lerror ("could not open file '%s'", src);
    
    FILE* fp_dest = fopen(dest, "w+");
    if (fp_dest == NULL)
        return lerror ("could not open file '%s' for writing", dest);
    
    int c = fgetc(fp_src);
    while (c != EOF)
    {
        fputc(c, fp_dest);
        c = fgetc(fp_src);
    }
    
    fclose (fp_src);
    fclose (fp_dest);
}