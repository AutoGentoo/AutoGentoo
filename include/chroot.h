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

typedef int _pid_c;

struct process_t {
    char command[256]; // Dont even parse anything (raw_command)

    _pid_c proc_id;

    pid_t process_id; // pid of the request fork()
    char *ip; // IP that requested
    int socket_fd;  // The socket file descriptor
                    // This fd will be closed immediatly and duped to stdout
                    // Only used as a buffer
};

struct chroot_client {
    struct manager * m_man;
    int sc_no; // Index of client
    struct process_t *proc_list[128];
    int proc_list_c;
    pid_t pid; // Process id of the chroot fork()
    int intited; // Specifies whether directories are mounted to chroot (/proc, /sys, /dev, /usr/portage)
};

//extern static const struct process_t NO_PROCESS;

void handle_sig_USR1 (int sig); // Handle process request
void handle_sig_USR2 (int sig); // Kill process

response_t process_handle (struct process_t*);
response_t process_kill (_pid_c);

struct chroot_client* chroot_new (struct manager* m_man, int sc_no);
void eselect_locale (char* loc);
void chroot_mount (struct chroot_client* client);
pid_t chroot_start (struct chroot_client* client);

_pid_c new_process (struct manager* m_man, int sc_no, char* request);

#endif
