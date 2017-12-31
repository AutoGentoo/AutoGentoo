#ifndef __AUTOGENTOO_STAGE_H__
#define __AUTOGENTOO_STAGE_H__

#include <response.h>

typedef struct __HostTemplate HostTemplate;

typedef enum {
    CXXFLAGS = 0x01,
    TMPDIR = 0x02,
    PORTDIR = 0x04,
    DISTDIR = 0x08,
    PKGDIR = 0x10,
    LOGDIR = 0x20
} template_selects;

struct __HostTemplate {
    char* id;
    char* arch;
    char* cflags;
    char* chost;
    char* make_extras;
    template_selects select;
    char* dest_dir; // Dont init
    Server* parent;
};

/**
 * Initialize a dynamic HostTemplate given a static one. \
 * Initilizes unknown fields such as dest_dir and parent
 * @param parent the parent server to bind to
 * @param t the static template to copy
 * @return a pointer the dynamically allocated template
 */
HostTemplate* host_template_init (Server* parent, HostTemplate t);

/**
 * Initialize stage3 given a HostTemplate
 * This downloads and extracts the a stage3 \
 * downloads are always from http://distfiles.gentoo.org/
 * @param t the HostTemplate to initilize
 */
void host_template_stage (HostTemplate* t);

/**
 * Mark The given HostTemplate as ready \
 * This allows the new Host to mount chroot and emerge new packages
 * @param dest The new parent (a Host) of the directory
 * @param src The old parent (a HostTemplate) of the directory
 * @return OK if all goes well, INTERNAL_ERROR if error occured
 */
response_t host_template_handoff (Host* dest, HostTemplate* src);

#endif