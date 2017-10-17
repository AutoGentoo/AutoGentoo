/*
 * chroot.h
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


#ifndef __AUTOGENTOO_CHROOT_H__
#define __AUTOGENTOO_CHROOT_H__

#include <stdio.h>
#include <serve_client.h>
#include <response.h>
#include <request.h>
#include <mntent.h>

struct chroot_client;

typedef enum mount_status {
    NOT_MOUNTED,
    IS_MOUNTED,
    NO_MOUNT, // If the parent directory is the child this is set
} mount_status;

struct chroot_mount {
    char parent[128]; // Relative to / of main (mount source)
    char child[128]; // Relative to chroot/ (mnt point)
    char type[32]; // Leave empty for auto
    int recursive; // 0 for --bind, 1 for --rbind (not used if type is specified)
};

struct chroot_client {
    struct chroot_mount mounts [16];
    struct manager * m_man;

    int sc_no; // Index of client
    int mount_c;
    int intited; // Specifies whether directories are mounted to chroot (/proc, /sys, /dev, /usr/portage)
};

struct chroot_client* chroot_new (struct manager* m_man, int sc_no);
void eselect_locale (char* loc, char* root);
void chroot_mount (struct chroot_client* client);

void type_mount (char* new_root, char* src, char* dest, char* type);
void bind_mount (char* new_root, char* src, char* dest, int recursive);
int get_mounts (struct manager* m_man, int sc_no, struct chroot_mount* mounts, int mount_c);

#endif
