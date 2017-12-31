#define _GNU_SOURCE
#include <stdio.h>
#include <stage.h>
#include <host.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <download.h>
#include <command.h>

static int stage_commands_init = 0;

SmallMap* tar = NULL;

void init_stage_commands (void) {
    tar = small_map_new (sizeof(Command*), 5);
    small_map_insert (tar, "extract", command_new("tar -xf %s", 1));
    small_map_insert (tar, "extract to", command_new("tar -xf %s -C %s", 2));
    small_map_insert (tar, "compress", command_new("tar -cf %s %s", 2));
}

void free_stage_commands (void) {
    int i;
    for (i = 0; i != tar->n; i++) {
        command_free ((Command*)(*(void***)vector_get(tar, i))[1]);
    }
    
    small_map_free(tar, 0);
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
    size_t len;
    char* stage3_dest = NULL;
    ssize_t read = 0;
    while ((read = getline(&line, &len, fp_temp)) != -1) {
        if (line[0] == '#')
            continue;
        // The first non-comment line will be our target stage3

        line[strlen(line) - 1] = '\0'; // Remove the newline
        char* s = strtok (line, " ");
        asprintf (&stage3_dest, "http://distfiles.gentoo.org/%s/autobuilds/%s", t->arch, s);
        break;
    }
    
    fclose (fp_temp);
    
    char* fname;
    asprintf (&fname, "%s/distfiles/stage3-%s.tar.bz2", t->parent->location, t->id);
    
    linfo ("Downloading stage3 from %s", stage3_dest);
    download (stage3_dest, fname, SHOW_PROGRESS);
    
    
}

response_t host_template_handoff (Host* dest, HostTemplate* src);
