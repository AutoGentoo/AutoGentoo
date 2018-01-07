//
// Created by atuser on 1/6/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "deps.h"

/**
 * @brief Generate depend struct from string
 * @param dep_str string to parse
 * @returns a dependency
 */
aabs_depend_t* aabs_dep_from_str (char* dep_str) {
    aabs_depend_t* out = malloc (sizeof (aabs_depend_t));
    memset (out, 0, sizeof (aabs_depend_t));
    
    char* desc, *ptr = NULL, *version;
    size_t dep_len;
    
    if ((desc = strstr(dep_str, ": ")) != NULL) {
        out->desc = strdup (desc + 2);
        dep_len = desc - dep_str;
        *desc = '\0';
    }
    else {
        out->desc = NULL;
        dep_len = strlen(dep_str);
    }
    
    /* Version selections */
    if((ptr = memchr(dep_str, '<', dep_len))) {
        if(ptr[1] == '=') {
            out->mod = AABS_DEP_MOD_LE;
            version = ptr + 2;
        }
        else {
            out->mod = AABS_DEP_MOD_LT;
            version = ptr + 1;
        }
    }
    else if((ptr = memchr(dep_str, '>', dep_len))) {
        if(ptr[1] == '=') {
            out->mod = AABS_DEP_MOD_GE;
            version = ptr + 2;
        } else {
            out->mod = AABS_DEP_MOD_GT;
            version = ptr + 1;
        }
    }
    else if((ptr = memchr(dep_str, '=', dep_len))) {
        out->mod = AABS_DEP_MOD_EQ;
        version = ptr + 1;
    }
    else {
        ptr = dep_str + dep_len;
        out->mod = AABS_DEP_MOD_ANY;
        out->version = NULL;
        version = NULL;
    }
    
    if (ptr)
        *ptr = '\0';
    
    out->name = strdup (dep_str);
    if (version)
        out->version = strdup (version);
    
    return out;
}

