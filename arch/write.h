//
// Created by atuser on 1/6/18.
//

#ifndef AUTOGENTOO_WRITE_H
#define AUTOGENTOO_WRITE_H

#include <stdio.h>
#include "aabs.h"

void write_dest_section (FILE* fp, char* name, char* value);
void write_dest_vector (FILE* fp, char* name, aabs_svec_t* vec);

#endif //AUTOGENTOO_WRITE_H
