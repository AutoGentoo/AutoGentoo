#define _GNU_SOURCE

#include <stdio.h>
#include <stage.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <download.h>
#include <command.h>
#include <unistd.h>

/*
 * Arguments (Everything after chost is optional)
 * [id] [portage-arch] [cflags] [chost] [extras]
 */

HostTemplate host_templates[] = {
        /*
        {"alpha", "alpha", "-mieee -pipe -O2 -mcpu=ev4", "alpha-unknown-linux-gnu", 1, {"/space/catalyst/portage", PORTDIR}},
        {"amd64", "amd64", "-O2 -pipe", "x86_64-pc-linux-gnu", 1, {"CPU_FLAGS_X86=\"mmx sse sse2\"", OTHER}},
        */
        "amd64-systemd", "amd64", "-O2 -pipe", "x86_64-pc-linux-gnu", 1, "CPU_FLAGS_X86=\"mmx sse sse2\"", OTHER,
        /*  
        {"armv4tl", "arm"},
        {"armv5tel", "arm"},
        {"armv6j", "arm"},
        {"armv6j_hardfp", "arm"},
        {"armv7a", "arm"},
        {"armv7a_hardfp", "arm", "-O2 -pipe -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=hard", "armv7a-hardfloat-linux-gnueabi"},
        {"arm64", "arm", "-O2 -pipe", "aarch64-unknown-linux-gnu"},
        {"hppa", "hppa", "-O2 -pipe -march=1.1", "hppa1.1-unknown-linux-gnu", "CXXFLAGS=\"-O2 -pipe\"", CXXFLAGS},
        */
};

StringVector* host_template_get_all() {
    StringVector* out = string_vector_new();
    int i;
    for (i = 0; i != sizeof(host_templates) / sizeof(host_templates[0]); i++) {
        string_vector_add(out, host_templates[i].id);
    }
    
    return out;
}

HostTemplate* host_template_init(Server* parent, HostTemplate t) {
    HostTemplate* out = malloc(sizeof(HostTemplate));
    out->parent = parent;
    
    out->id = strdup(t.id);
    out->arch = strdup(t.arch);
    out->cflags = strdup(t.cflags);
    out->chost = strdup(t.chost);
    
    int i;
    for (i = 0; i != t.extra_c; i++) {
        out->extras[i].make_extra = strdup(t.extras[i].make_extra);
        out->extras[i].select = t.extras[i].select;
    }
    
    out->new_id = host_id_new();
    asprintf(&out->dest_dir, "%s/stage-%s", parent->location, out->new_id);
    
    small_map_insert(parent->stages, out->new_id, out);
    
    return out;
}

void host_template_stage(HostTemplate* t) {
    char distfile_dir[256];
    sprintf (distfile_dir, "%s/distfiles", t->parent->location);
    
    if (!opendir(distfile_dir)) {
        mkdir(t->dest_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    
    char distfile_meta_url[256];
    sprintf (distfile_meta_url, "http://distfiles.gentoo.org/%s/autobuilds/latest-stage3-%s.txt", t->arch, t->id);
    
    if (download(distfile_meta_url, "temp_dest", NO_PROGRESS).code != 200) {
        lerror("Could not download metadata for stage3!");
        return;
    }
    
    FILE* fp_temp = fopen("temp_dest", "r");
    
    char* line;
    size_t len;
    char* stage3_dest = NULL;
    ssize_t read = 0;
    char stage3_date[32];
    while ((read = getline(&line, &len, fp_temp)) != -1) {
        if (line[0] == '#')
            continue;
        // The first non-comment line will be our target stage3
        
        line[strlen(line) - 1] = '\0'; // Remove the newline
        char* s = strtok(line, " ");
        strcpy(stage3_date, s);
        s = strtok(NULL, " ");
        asprintf(&stage3_dest, "http://distfiles.gentoo.org/%s/autobuilds/%s", t->arch, s);
        break;
    }
    fclose(fp_temp);
    remove("temp_dest");
    
    char* fname;
    asprintf(&fname, "%s/distfiles/stage3-%s-%s.tar.bz2", t->parent->location, t->id, stage3_date);
    
    if (access(fname, F_OK) == -1) {
        linfo("Downloading stage3 from %s", stage3_dest);
        download(stage3_dest, fname, SHOW_PROGRESS);
        free(stage3_dest);
    }
    
    int ext_ret;
    command("tar", "extract to", NULL, &ext_ret, fname, t->dest_dir);
    
    if (ext_ret != 0)
        lerror("Failed to extract stage3 tar");
}

Host* host_template_handoff(HostTemplate* src) {
    Host* out = host_new(src->parent, src->id);
    char host_dir[256];
    host_get_path(out, host_dir);
    if (rename(src->dest_dir, host_dir) != 0) {
        lerror("Failed to rename %s to %s", src->dest_dir, host_dir);
        host_free(out);
        return NULL;
    }
    strcpy (out->hostname, "default");
    
    // Profile
    {
        char* t_profile_l;
        char profile_dest[256];
        
        asprintf(&t_profile_l, "%s/etc/portage/make.profile", src->dest_dir);
        ssize_t profile_len = readlink(t_profile_l, profile_dest, sizeof(profile_dest) - 1);
        profile_dest[(int)profile_len] = 0; // Readlink does not null terminal
        free(t_profile_l);
        
        char* t_profile_split = strstr(profile_dest, "profiles/");
        free(out->profile);
        out->profile = strdup(t_profile_split + strlen("profiles/"));
    }
    
    // defaults
    {
        string_overwrite(&out->cxxflags, "${CFLAGS}", 1);
        string_overwrite(&out->portage_tmpdir, "/autogentoo/tmp", 1);
        string_overwrite(&out->portdir, "/usr/portage", 1);
        string_overwrite(&out->distdir, "/usr/portage/distfiles", 1);
        string_overwrite(&out->pkgdir, "/autogentoo/pkg", 1);
        string_overwrite(&out->port_logdir, "/autogentoo/log", 1);
    }
    
    // make.conf stuff
    {
        strcpy (out->cflags, src->cflags);
        strcpy (out->arch, src->arch);
        strcpy (out->chost, src->chost);
    }
    
    struct {
        template_selects sel;
        char* ptr;
    } _t[] = {
            {OTHER, NULL},
            {CXXFLAGS, out->cxxflags},
            {TMPDIR,   out->portage_tmpdir},
            {PORTDIR,  out->portdir},
            {DISTDIR,  out->distdir},
            {PKGDIR,   out->pkgdir},
            {LOGDIR,   out->port_logdir}
    };
    
    int i;
    for (i = 0; i != src->extra_c; i++) {
        if (src->extras[i].select == OTHER) {
            string_vector_add(out->extra, src->extras[i].make_extra);
            continue;
        }
        
        int j;
        for (j = 0; j != sizeof(_t) / sizeof(_t[0]); j++) {
            if (src->extras[i].select == _t[j].sel) {
                strcpy (_t[j].ptr, src->extras[i].make_extra);
                break;
            }
        }
    }
    
    src = small_map_delete(src->parent->stages, src->new_id);
    host_template_free(src);
    return out;
}

void host_template_free(HostTemplate* temp) {
    free(temp->id);
    free(temp->arch);
    free(temp->cflags);
    free(temp->chost);
    
    int i;
    for (i = 0; i != temp->extra_c; i++) {
        free(temp->extras->make_extra);
    }
    
    free(temp->dest_dir);
    free(temp->new_id);
    free(temp);
}