//
// Created by atuser on 11/14/17.
//


#ifndef HACKSAW_USE_FLAGS_H
#define HACKSAW_USE_FLAGS_H

typedef struct __use_flag UseFlag;

typedef enum {
    FLAG_DISABLED,
    FLAG_ENABLED
} use_t;

struct __use_flag {
    
    use_t status;
}

#endif