#include <stdio.h>
#include <stage.h>
#include <host.h>
#include <stdlib.h>
#include <string.h>

HostTemplate host_templates[] = {
    {"alpha", "alpha", "-mieee -pipe -O2 -mcpu=ev4", "alpha-unknown-linux-gnu", "PORTDIR=\"/space/catalyst/portage\"", PORTDIR},
    {"amd64", "amd64", "-O2 -pipe", "x86_64-pc-linux-gnu", "CPU_FLAGS_X86=\"mmx sse sse2\""},
    {"armv4tl", "arm"},
    {"armv5tel", "arm"},
    {"armv6j", "arm"},
    {"armv6j_hardfp", "arm"},
    {"armv7a", "arm"},
    {"armv7a_hardfp", "arm", "-O2 -pipe -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=hard", "armv7a-hardfloat-linux-gnueabi"},
    {"arm64", "arm64", "-O2 -pipe", "aarch64-unknown-linux-gnu"},
    {"hppa", "hppa", "-O2 -pipe -march=1.1", "hppa1.1-unknown-linux-gnu", "CXXFLAGS=\"-O2 -pipe\"", CXXFLAGS},
};

HostTemplate* host_template_init (Server parent, HostTemplate t) {
    HostTemplate* out = malloc (sizeof(HostTemplate));
    out->parent = parent;
    memcpy (out, &t, sizeof (HostTemplate));
    
    sprintf (out->dest_dir, "%s/stage-%s", parent->location, t.arch);
    
    if (opendir(out->dest_dir)) {
        lerror ("Path %s exists, wait-until the queued stage is handed-off", out->dest_dir);
        lerror ("You can remove this directory if you no longer need it");
        free (out);
        return NULL;
    }
    
    return out;
}

void host_template_stage (HostTemplate* t) {
    char distfile_dir[256];
    sprintf ("%s/distfiles", t->parent->location);
    
    if (!opendir(distfile_dir)) {
        mkdir(t->dest_dir);
    }
    
    
}

response_t host_template_handoff (Host* dest, HostTemplate* src);