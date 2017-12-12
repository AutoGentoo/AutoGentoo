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

HostTemplate* host_template_init (Server* parent, HostTemplate t);
void host_template_stage (HostTemplate* t);
response_t host_template_handoff (Host* dest, HostTemplate* src);

#endif