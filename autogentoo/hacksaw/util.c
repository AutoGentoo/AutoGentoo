#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include "log.h"
#include "util.h"

void prv_mkdir(const char* dir)
{
    DIR* _dir = opendir(dir);
    if (_dir)
    {
        closedir(_dir);
        return;
    }
    char tmp[256];
    char* p = NULL;

    snprintf(tmp, sizeof(tmp), "%s", dir);
    U32 len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++)
    {
        if (*p == '/')
        {
            *p = 0;
            mkdir(tmp, 0777);
            *p = '/';
        }
    }
    mkdir(tmp, 0777);
}

void file_copy(char* src, char* dest)
{
    FILE* fp_src = fopen(src, "r");
    if (fp_src == NULL)
        return lerror("could not open file '%s'", src);

    FILE* fp_dest = fopen(dest, "w+");
    if (fp_dest == NULL)
    {
        fclose(fp_src);
        return lerror("could not open file '%s' for writing", dest);
    }

    int c = fgetc(fp_src);
    while (c != EOF)
    {
        fputc(c, fp_dest);
        c = fgetc(fp_src);
    }

    fclose(fp_src);
    fclose(fp_dest);
}

int string_find(char** array, char* element, U32 n)
{
    for (U32 i = 0; i != n; i++)
        if (strcmp(element, array[i]) == 0)
            return i;
    return -1;
}

void fix_path(char* ptr)
{
    size_t n = strlen(ptr);
    for (U32 i = 1; i != n; i++)
    {
        if (ptr[i] == '/' && ptr[i - 1] == '/')
        {
            strcpy(&ptr[i - 1], &ptr[i]);
            ptr[n - 1] = 0;
            n--;
        }
    }
}

char* string_strip(char* str)
{
    size_t len = strlen(str);

    U32 i = 0;
    for (; i < len && str[i] == ' '; i++);

    U32 j = len;
    for (; j > i && str[j] == ' '; j--);

    return strndup(str + i, (U32) j - i);
}

static void ref_string_free(RefData* self)
{
    free(self->ptr);
    free(self);
}

static void ref_pyobect_free(RefData* self)
{
    Py_XDECREF(self->ptr);
    free(self);
}

RefObject* ref_string(char* str)
{
    RefData* self = malloc(sizeof(RefData));
    self->free = (void (*)(void*)) ref_string_free;
    self->ptr = str;
    self->reference_count = 0;

    return (RefObject*) self;
}

RefObject* ref_pyobject(PyObject* data)
{
    RefData* self = malloc(sizeof(RefData));
    self->free = (void (*)(void*)) ref_pyobect_free;

    Py_XINCREF(data);
    self->ptr = data;
    self->reference_count = 0;

    return (RefObject*) self;
}