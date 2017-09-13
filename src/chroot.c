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

    response_t res = process_handle (process_buffer);
    rsend (process_buffer->socket_fd, res);
    munmap (process_buffer, sizeof (struct process_t));
}

void handle_sig_USR2 (int sig) {
    ;
}

response_t process_handle (struct process_t* proc) {

}

response_t process_kill (_pid_c pid_kill) {
    if (kill (pid_kill, SIGKILL) != 0) {
        return INTERNAL_ERROR;
    }
    return OK;
}

struct chroot_client* chroot_new (struct manager* m_man, int sc_no) {
    struct chroot_client* chroot = (struct chroot_client*) mmap(NULL, sizeof (struct chroot_client), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    chroot->m_man = m_man;
    chroot->sc_no = sc_no;
    chroot->proc_list_c = 0;
    chroot->pid = -1;
    chroot->intited = 0;
    
    return chroot;
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
    struct serve_client * buffer_client = &client->m_man->clients[client->sc_no];
    
    sprintf (target, "%s/%s", client->m_man->root, buffer_client->id);
    type_mount (target, "/proc", "proc", "proc");
    bind_mount (target, "/sys", "sys", 1);
    bind_mount (target, "/dev", "dev", 1);
    bind_mount (target, buffer_client->PORTAGE_DIR, 
                        buffer_client->PORTDIR, 0);
    
    client->intited = 1;
    
    char resolv_conf_chroot [256];
    sprintf (resolv_conf_chroot, "%s/etc/resolv.conf", client->m_man->root);
    cpy (buffer_client->resolv_conf, resolv_conf_chroot);
    eselect_locale (buffer_client->locale);
    
    // ldconfig
    // locale.gen
    // Move target/autogentoo/etc/portage to /etc/portage
    // Make SYSROOT in make.conf / because we use chroot now
    // Link etc/resolv.conf
}

void eselect_locale (char* loc) {
    char buffer[256];
    sprintf (buffer, "# Configuration file for eselect\n\
# This file has been automatically generated.\n\
LANG=\"%s\"", loc);
    
    FILE * fp;
    fp = fopen (loc, "w+");
    if (fp != NULL)
    {
        fputs(buffer, fp);
        fclose(fp);
    }
}

pid_t chroot_start (struct chroot_client* client) {
    pid_t buf_pid = fork ();
    if (buf_pid == -1) {
        exit (1);
    }
    if (buf_pid > 0) {
        return (client->pid = buf_pid);
    }
    // Inside the chroot fork;
    char __ROOT[256];
    sprintf (__ROOT, "%s/%s/", client->m_man->root, client->m_man->clients[client->sc_no].id);
    if (chroot (__ROOT) == -1) {
        exit (1);
    }
    system ("ls /");
    // Handle signals given by m_install and m_remove
    signal(SIGUSR1, handle_sig_USR1);
    signal(SIGUSR2, handle_sig_USR2);
    while (1) {
        pause (); // Wait for signal
    }
}

_pid_c new_process (struct manager* m_man, int sc_no, char* request);
