//
// Created by atuser on 1/7/18.
//

#include "ini.h"
#include "string.h"

aabs_ini_t* aabs_ini_parse (char* path) {
    return conf_new(path);
}
aabs_svec_t* aabs_ini_getsections (aabs_ini_t* ini) {
    aabs_svec_t* out = string_vector_new ();
    
    int i;
    for (i = 0; i != ini->sections->n; i++)
        string_vector_add(out, (*(aabs_inisec_t**)vector_get (ini->sections, i))->name);
    
    return out;
}

char* aabs_ini_getvariable(aabs_ini_t* ini, char* section, char* varname) {
    return conf_get(ini, section, varname);
}

char* aabs_ini_getsecvariable(aabs_inisec_t* sec, char* varname) {
    int i;
    for (i = 0; i!= sec->variables->n; i++) {
        aabs_inivar_t* curr_var = *(aabs_inivar_t**)vector_get(sec->variables, i);
        if (strcmp (curr_var->identifier, varname) == 0)
            return curr_var->value;
    }
    
    return NULL;
}