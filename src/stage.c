#define _GNU_SOURCE
#include <stdio.h>
#include <stage.h>
#include <host.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <archive.h>
#include <archive_entry.h>
#include <download.h>

static int prv_copy_data (struct archive *ar, struct archive *aw) {
    int r;
    const void *buff;
    size_t size;
    la_int64_t offset;

    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF)
            return (ARCHIVE_OK);
        if (r < ARCHIVE_OK)
            return (r);
        r = archive_write_data_block(aw, buff, size, offset);
        if (r < ARCHIVE_OK) {
        fprintf(stderr, "%s\n", archive_error_string(aw));
            return (r);
        }
    }
}

response_t prv_extract_stage3 (HostTemplate* t) {
    char fname[256];
    sprintf (fname, "%s/distfiles/stage3-%s.tar.bz2", t->parent->location, t->id);
    
    if (access(fname, F_OK) == -1) {
        lerror ("distfile 'stage3-%s.tar.bz2' does not exist (please download)", t->id);
        return INTERNAL_ERROR;
    }

    struct archive* a;
    struct archive* ext;
    struct archive_entry* entry;
    int flags;
    int r;

    /* Select which attributes we want to restore. */
    flags = ARCHIVE_EXTRACT_TIME;
    flags |= ARCHIVE_EXTRACT_PERM;
    flags |= ARCHIVE_EXTRACT_ACL;
    flags |= ARCHIVE_EXTRACT_FFLAGS;
    
    a = archive_read_new();
    archive_read_support_format_all(a);
    archive_read_support_compression_all(a);
    ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    archive_write_disk_set_standard_lookup(ext);
    if ((r = archive_read_open_filename(a, fname, 10240)))
        return INTERNAL_ERROR;
    for (;;) {
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF)
            break;
        if (r < ARCHIVE_OK)
            fprintf(stderr, "%s\n", archive_error_string(a));
        if (r < ARCHIVE_WARN)
            return INTERNAL_ERROR;
        r = archive_write_header(ext, entry);
        if (r < ARCHIVE_OK)
            fprintf(stderr, "%s\n", archive_error_string(ext));
        else if (archive_entry_size(entry) > 0) {
            r = prv_copy_data(a, ext);
            if (r < ARCHIVE_OK)
                fprintf(stderr, "%s\n", archive_error_string(ext));
            if (r < ARCHIVE_WARN)
                return INTERNAL_ERROR;
        }
        r = archive_write_finish_entry(ext);
        if (r < ARCHIVE_OK)
            fprintf(stderr, "%s\n", archive_error_string(ext));
        if (r < ARCHIVE_WARN)
            return INTERNAL_ERROR;
    }
    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);
    
    return OK;
}

HostTemplate host_templates[] = {
    {"alpha", "alpha", "-mieee -pipe -O2 -mcpu=ev4", "alpha-unknown-linux-gnu", "PORTDIR=\"/space/catalyst/portage\"", PORTDIR},
    {"amd64", "amd64", "-O2 -pipe", "x86_64-pc-linux-gnu", "CPU_FLAGS_X86=\"mmx sse sse2\""},
    {"amd64-systemd", "amd64", "-O2 -pipe", "x86_64-pc-linux-gnu", "CPU_FLAGS_X86=\"mmx sse sse2\""},
    {"armv4tl", "arm"},
    {"armv5tel", "arm"},
    {"armv6j", "arm"},
    {"armv6j_hardfp", "arm"},
    {"armv7a", "arm"},
    {"armv7a_hardfp", "arm", "-O2 -pipe -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=hard", "armv7a-hardfloat-linux-gnueabi"},
    {"arm64", "arm", "-O2 -pipe", "aarch64-unknown-linux-gnu"},
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
    sprintf (distfile_dir, "%s/distfiles", t->parent->location);
    
    if (!opendir(distfile_dir)) {
        mkdir(t->dest_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    
    char distfile_meta_url[256];
    sprintf (distfile_meta_url,"http://distfiles.gentoo.org/%s/autobuilds/latest-stage3-%s.txt", t->arch, t->id);
    
    if (download (distfile_meta_url, "temp_dest", NO_PROGRESS).code != 200) {
        lerror ("Could not download metadata for stage3!");
        return;
    }
    
    FILE* fp_temp = fopen ("temp_dest", "r");
    
    char* line;
    ssize_t len;
    char* stage3_dest;
    size_t read = 0;
    while ((read = getline(&line, &len, fp_temp)) != -1) {
        if (line[0] == '#')
            continue;
        line[strlen(line) - 1] = '\0'; // Remove the newline
        char* s = strtok (line, " ");
        asprintf (&stage3_dest, "http://distfiles.gentoo.org/%s/autobuilds/%s", t->arch, s);
        break;
    }
    
    fclose (fp_temp);
    
    char fname[256];
    sprintf (fname, "%s/distfiles/stage3-%s.tar.bz2", t->parent->location, t->id);
    
    linfo ("Downloading stage3 from %s", stage3_dest);
    download (stage3_dest, fname, SHOW_PROGRESS);
}



response_t host_template_handoff (Host* dest, HostTemplate* src);
