#include <stdio.h>
#include <stage.h>
#include <host.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <archive.h>
#include <archive_entry.h>

response_t prv_extract_stage3 (HostTemplate* t) {
    char fname[256];
    sprintf (fname, "%s/distfiles/stage3-%s.tar.bz2", t->parent->location, t->id);
    
    if (access(fname, F_OK) == -1) {
        lerror ("distfile 'stage3-%s.tar.bz2' does not exist (please download)", t->id);
        return INTERNAL_ERROR;
    }
    
    int r;
    ssize_t size;

    struct archive *a = archive_read_new();
    archive_read_support_compression_all(a);
    archive_read_support_format_raw(a);
    r = archive_read_open_filename(a, fname, 16384);
    
    if (r != ARCHIVE_OK) {
        archive_read_free (a);
        return INTERNAL_ERROR;
    }
    
    r = archive_read_next_header(a, &ae);
    if (r != ARCHIVE_OK) {
        archive_read_free (a);
        return INTERNAL_ERROR;
    }

    for (;;) {
        size = archive_read_data(a, buff, buffsize);
        if (size < 0) {
            archive_read_free (a);
            return INTERNAL_ERROR;
        }
        if (size == 0)
            break;
        write(1, buff, size);
    }

    archive_read_free(a));
    return OK;
}

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

HostTemplate* host_template_init (Server* parent, HostTemplate t) {
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
        mkdir(t->dest_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    
    
}



response_t host_template_handoff (Host* dest, HostTemplate* src);
