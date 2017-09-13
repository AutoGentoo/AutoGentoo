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

static int* current_proc_id;
static struct process_t* process_buffer = NULL;

void handle_sig_USR1 (int sig) { // Handle process request
    pid_t buf_pid = fork ();
    if (buf_pid == -1) {
        exit (-1);
    }
    if (buf_pid > 0) {
        process_buffer->process_id = buf_pid;
        return;
    }
    // Inside process

    res = process_handle (process_buffer);
    rsend (process_buffer->socket_fd, res);
    munmap (process_buffer, sizeof struct process_t);
}

response_t process_handle (struct process_t* proc) {

}

response_t process_kill (_pid_c pid_kill) {
    if (kill (pid_kill, SIGKILL) != 0) {
        return INTERNAL_ERROR;
    }
    return OK;
}

void chroot_new (struct manager* m_man, int sc_no) {
    struct chroot_client* chroot = m_man = mmap(NULL, sizeof (struct chroot_client), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    chroot->m_man = m_man;
    chroot->sc_no = sc_no;
    chroot->proc_list_c = 0
    chroot->pid = -1;

    m_man->clients[sc_no].chroot = chroot;
}

void bind_mount (char* new_root, char* src, char* dest, int recursive) {
    char dest_temp[256];
    sprintf (dest_temp, "%s/%s", new_root, dest);
    mount (src, dest_temp, NULL, MS_MGC_VAL | MS_BIND | (recursive ? MS_REC : 0), NULL);
}
void type_mount (char* new_root, char* src, char* dest, char* type) {
    char dest_temp[256];
    sprintf (dest_temp, "%s/%s", new_root, dest);
    mount (src, dest_temp, type, MS_MGC_VAL, NULL);
}

void chroot_mount (struct chroot_client* client) {
    char target[256];
    sprintf (target, "%s/%s", client->m_man.root, client->m_man.clients[client->sc_no].id);
    type_mount (target, "/proc", "proc", "proc");
    bind_mount (target, "/sys", "sys", 1);
    bind_mount (target, "/dev", "dev", 1);

    // ldconfig
    // locale.gen
    // mount --rbind /usr/portage target/usr/portage
    // Move target/autogentoo/etc/portage to /etc/portage
    // Make SYSROOT in make.conf / because we use chroot now
    // Link etc/resolv.conf
}

pid_t chroot_start (struct chroot_client* client) {
    buf_pid = fork ();
    if (buf_pid == -1) {
        exit (1);
    }
    if (buf_pid > 0) {
        return (client->pid = buf_pid);
    }
    // Inside the chroot fork;

    // Handle signals given by m_install and m_remove
    signal(SIGUSR1, handle_sig_USR1);
    signal(SIGUSR2, handle_sig_USR2);
    while (1) {
        pause (); // Wait for signal
    }
}

_pid_c new_process (struct manager* m_man, int sc_no, char* request);
