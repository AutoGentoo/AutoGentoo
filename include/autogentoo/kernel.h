//
// Created by Andrei Tumbar on 1/4/18.
//

#ifndef __AUTOGENTOO_KERNEL_H__
#define __AUTOGENTOO_KERNEL_H__

#include "host.h"

typedef struct __Kernel Kernel;

struct __Kernel {
    Host* parent;
    
    /**
     * @brief The suffix of the kernel
     * 
     * For example -ck or -gentoo
     * Use empty if it is the vanilla kernel
     */
    char* kernel_target;
    
    /**
     * The version of this kernel
     * (ie: 2.6.32)
     */
    char* version;

    /**
     * The URI of the kernel git repo
     * NULL if you dont want to use git
     */
    char* uri;

    /**
     * @brief Root of the kernel sources
     *
     * Default will be /usr/src/linux-{version}{kernel-target}
     */
    char* kroot;
};

Kernel* kernel_new (Host* parent, char* target, char* version);
void kernel_set_uri (Kernel* k, char* uri);
void kernel_sync (Kernel* k);
void kernel_free (Kernel* k);

#endif //__AUTOGENTOO_KERNEL_H__
