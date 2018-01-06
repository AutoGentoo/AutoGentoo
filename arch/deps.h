//
// Created by atuser on 1/6/18.
//

#ifndef AUTOGENTOO_DEPENDENCY_H
#define AUTOGENTOO_DEPENDENCY_H

typedef struct _aabs_depend_t aabs_depend_t;

typedef enum {
    AABS_DEP_MOD_ANY = 1,
    AABS_DEP_MOD_EQ,
    AABS_DEP_MOD_GE,
    AABS_DEP_MOD_LE,
    AABS_DEP_MOD_GT,
    AABS_DEP_MOD_LT
} aabs_depmod_t;

typedef struct _aabs_depend_t {
    char *name;
    char *version;
    char *desc;
    unsigned long name_hash;
    aabs_depmod_t mod;
};

aabs_depend_t* aabs_dep_from_str (char* dep_str);

#endif //AUTOGENTOO_DEPENDENCY_H
