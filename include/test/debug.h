//
// Created by atuser on 10/22/17.
//

#ifndef HACKSAW_DEBUG_H_H
#define HACKSAW_DEBUG_H_H

#include <stddef.h>
#include <tools/vector.h>
#include <tools/string_vector.h>
#include <config/conf.h>

void print_bin (void* ptr, int n, size_t size);
void print_vec(Vector* vec);
void print_string_vec(StringVector* vec);
void print_conf (Conf* config);


#endif //HACKSAW_DEBUG_H_H
