#ifndef __AUTOGENTOO_CHROOT_H__
#define __AUTOGENTOO_CHROOT_H__

#include "response.h"

/**
 * @brief Information about mount point for a host
 */
typedef struct __ChrootMount ChrootMount;

/**
 * @brief Information about mount point for a host
 */
struct __ChrootMount {
    char* src; //!< The place to mount from
    char* dest; //!< The place to mount to
    char* type; //!< The type of mount, NULL for default/auto
    unsigned long opts; //!< The options to pass to the mount (look in mount.h)
};

/**
 * Mount all the directories specified by the Host
 * @param host the host for which the directory wll be mounted
 * @return OK if all goes well, INTERNAL_ERROR if error occures
 */
response_t chroot_mount(Host* host);

#endif