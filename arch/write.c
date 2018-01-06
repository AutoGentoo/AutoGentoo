//
// Created by atuser on 1/6/18.
//

#include "write.h"
#include "aabs.h"

void write_dest_section (FILE* fp, char* name, char* value) {
    fprintf (fp, "%%%s%%\n%s\n\n", name, value);
}

void write_dest_vector (FILE* fp, char* name, aabs_svec_t* vec) {
    fprintf (fp, "%%%s%%", name);
    
    int i;
    for (i = 0; i != vec->n; i++) {
        fprintf (fp, "%s\n", string_vector_get(vec, i));
    }
    
    fwrite ("\n", 1, 1, fp);
}