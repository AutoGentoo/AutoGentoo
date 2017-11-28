#include <chroot.h>
#include <sys/mount.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <mntent.h>
#include "host.h"

void prv_get_mounted (StringVector* dest) {
    struct mntent* fs;
    FILE* fp = setmntent ("/proc/mounts", "r");
    if (fp == NULL) {
        lerror ("Could not open /proc/mounts!");
        exit (1);
    }
    
    while ((fs = getmntent(fp)) != NULL)
        string_vector_add (dest, fs->mnt_dir);
    endmntent(fp);
}

response_t chroot_mount (Host* host) {
    ChrootMount to_mount[] = {
        {"/usr/portage", "usr/portage", NULL, MS_BIND},
        {"/dev", "dev", NULL, MS_BIND | MS_REC},
        {"/sys", "sys", NULL, MS_BIND | MS_REC},
        {"/proc", "proc", "proc", MS_MGC_VAL}
    };
    
    StringVector* mounted = string_vector_new ();
    prv_get_mounted (mounted);
    
    char new_root[256];
    host_get_path(host, (char*)new_root);
    
    int i;
    char* dest_temp;
    for (i = 0; i != sizeof (to_mount) / sizeof (to_mount[0]); i++) {
        ChrootMount mnt = to_mount[i];
        
        dest_temp = malloc (strlen (new_root) + strlen(mnt.dest) + 2);
        sprintf (dest_temp, "%s/%s", new_root, mnt.dest);
        fix_path (dest_temp);
        
        if (string_find (mounted->ptr, dest_temp, mounted->n) == -1) {
            /*
            if (mount (mnt.src, dest_temp, mnt.type, mnt.opts, NULL) == -1) {
                lerror ("Failed to mount %s", dest_temp);
                string_vector_free (mounted);
                free (dest_temp);
                host->chroot_status = CHR_NOT_MOUNTED;
                return INTERNAL_ERROR;
            }*/
            lwarning ("mounting %s to %s", mnt.src, dest_temp);
        }
        free (dest_temp);
    }
    
    string_vector_free (mounted);
    host->chroot_status = CHR_MOUNTED;
    return OK;
}