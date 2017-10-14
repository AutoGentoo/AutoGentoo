/*
 * chroot.c
 *
 * Copyright 2017 Unknown <atuser@Hyperion>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */


#include <stdio.h>
#include <chroot.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/mman.h>
#include <_string.h>

static struct system_mounts* sys_mnts;

int mount_to_sc (struct manager* m_man, char** client_roots, char* path) {
    int i;
    for (i = 0; i != m_man->client_c; i++) {
        if (strncmp (client_roots[i], path, strlen(client_roots[i])) == 0) {
            
        }
    }
    
    char check_id[32];
    sscanf (path, "^(.*/)([^/]*)$", NULL, check_id);
}

void chroot_main (struct manager* m_man) {
    sys_mnts = (struct system_mounts*) mmap(NULL, sizeof (struct system_mounts), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    struct mntent *ent;
    FILE *mnts_stream;
    
    mnts_stream = setmntent("/proc/mounts", "r");
    if (mnts_stream == NULL) {
        perror("setmntent");
        exit(1);
    }
    while ((ent = getmntent(mnts_stream)) != NULL) {
        strcpy (sys_mnts->mounts[sys_mnts->mount_c], ent->mnt_dir);
        sys_mnts->mount_c++;
    }
    endmntent(mnts_stream);
}

struct chroot_client* chroot_new (struct manager* m_man, int sc_no) {
    struct chroot_client* chroot = (struct chroot_client*) mmap(NULL, sizeof (struct chroot_client), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    chroot->m_man = m_man;
    chroot->sc_no = sc_no;
    chroot->proc_c = 0;
    
    chroot->pid = -1;
    chroot->intited = 0;
    
    struct serve_client * buffer_client = &m_man->clients[sc_no];
    
    char PORTAGE_DIR[128];
    char PORTDIR[128];
    strcpy (PORTAGE_DIR, buffer_client->PORTAGE_DIR);
    strcpy (PORTDIR, buffer_client->PORTDIR);
    
    struct chroot_mount mounts[4] = {
        {"/proc", "proc", "proc", 0},
        {"/sys", "sys", "", 1},
        {"/dev", "dev", "", 1},
        {"/usr/portage", "usr/portage", "", 0}
    };
    
    memcpy (&chroot->mounts, &mounts, sizeof (struct chroot_mount) * 4);
    chroot->mount_c = 4;
    
    return chroot;
}

void bind_mount (char* new_root, char* src, char* dest, int recursive) {
    char dest_temp[2048];
    sprintf (dest_temp, "%s/%s", new_root, dest);
    strcpy (dest_temp, path_normalize (dest_temp));
    mount (src, dest_temp, NULL, MS_BIND | (recursive ? MS_REC : 0), NULL);
}
void type_mount (char* new_root, char* src, char* dest, char* type) {
    char dest_temp[2048];
    sprintf (dest_temp, "%s/%s", new_root, dest);
    mount (src, dest_temp, type, MS_MGC_VAL, NULL);
}

int get_mounts (struct manager* m_man, int sc_no, struct chroot_mount* mounts, int mount_c) {
    int out = 0; // Zero out all of the bits
    char sc_root[256];
    sprintf (sc_root, "%s/%s", m_man->root, m_man->clients[sc_no].id);
    strcpy (sc_root, path_normalize(sc_root));
    int sc_root_l = strlen(sc_root);
    
    int i;
    for (i = 0; i != sys_mnts->mount_c; i++) {
        if (strncmp (sys_mnts->mounts[i], sc_root, sc_root_l) != 0)
            continue;
        
        // We know that the current mount path is part of this client
        // Find which mount this is pointing to
        char dest_temp[2048];
        int j;
        for (j = 0; j != mount_c; j++) {
            sprintf (dest_temp, "%s/%s", sc_root, mounts[j].child);
            strcpy (dest_temp, path_normalize (dest_temp));
            
            if (strcmp (sys_mnts->mounts[i], dest_temp) == 0) {
                out |= 1 << j;
                break;
            }
        }
    }
    
    return out;
}

int mount_check (struct chroot_mount mnt, char* target) {
    /*
    if (strcmp (src_temp, dest_temp) == 0) {
        return 1;
    }
    
    int i;
    for (i=0; i!=sys_mnts->mount_c; i++) {
        if (strcmp (dest_temp, sys_mnts->mounts[i]) == 0) {
            return 1;
        }
    }*/
    return 0;
}


void chroot_mount (struct chroot_client* client) {
    char target[256];
    struct serve_client * buffer_client = &client->m_man->clients[client->sc_no];
    
    sprintf (target, "%s/%s", client->m_man->root, buffer_client->id);
    
    int i;
    for (i=0; i!=client->mount_c; i++) {
        mount_status got = mount_check (client->mounts[i], target);
        if (got != NOT_MOUNTED) {
            continue;
        }
        
        if (strcmp(client->mounts[i].type, "") == 0) {
            bind_mount (target, client->mounts[i].parent, client->mounts[i].child, client->mounts[i].recursive);
        }
        else {
            type_mount (target, client->mounts[i].parent, client->mounts[i].child, client->mounts[i].type);
        }
    }
    
    
    char resolv_conf_chroot [256];
    sprintf (resolv_conf_chroot, "%s/%s/etc/resolv.conf", client->m_man->root, buffer_client->id);
    cpy (buffer_client->resolv_conf, resolv_conf_chroot);
    eselect_locale (buffer_client->locale, client->m_man->root);
    
    client->intited = 1;
    
    // locale.gen
    // Move target/autogentoo/etc/portage to /etc/portage
    // Make SYSROOT in make.conf / because we use chroot now
    // Link etc/resolv.conf
}

void eselect_locale (char* loc, char* root) {
    char buffer[256];
    sprintf (buffer, "# Configuration file for eselect\n\
# This file has been automatically generated.\n\
LANG=\"%s\"", loc);
    
    FILE * fp;
    
    char path[256];
    sprintf (path, "%s/etc/locale.gen", root);
    
    fp = fopen (path, "a");
    if (fp != NULL)
    {
        fputs(buffer, fp);
        fclose(fp);
    }
}