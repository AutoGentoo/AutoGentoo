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
};



#endif //__AUTOGENTOO_KERNEL_H__
