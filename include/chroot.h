#ifndef __AUTOGENTOO_CHROOT_H__
#define __AUTOGENTOO_CHROOT_H__

#include "response.h"

typedef struct __ChrootMount ChrootMount;

struct __ChrootMount {
    char* src;
    char* dest;
    char* type; // NULL for default
    unsigned long opts;
};

response_t chroot_mount (Host* host);

#endif