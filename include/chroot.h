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

/**
 * Mount all the directories specified by the Host
 * @param host the host for which the directory wll be mounted
 * @return OK if all goes well, INTERNAL_ERROR if error occures
 */
response_t chroot_mount (Host* host);

#endif