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

volatile static int* current_proc_id;
volatile struct process_t* process_buffer = NULL;
static struct system_mounts* sys_mnts;

void handle_sig_USR1 (int sig) { // Handle process request
    pid_t buf_pid = fork ();
    if (buf_pid == -1) {
        exit (-1);
    }
    if (buf_pid > 0) {
        process_buffer->pid = buf_pid;
        return;
    }
    // Inside process
    process_buffer->status = RUNNING;
    response_t res = process_handle ((struct process_t*)process_buffer);
    rsend (process_buffer->socket_fd, res);
    process_buffer->status = DEFUNCT;
    process_buffer = NULL;
    exit (0);
}

void handle_sig_USR2 (int sig) {
    if (process_buffer->status != RUNNING) {
        rsend (process_buffer->socket_fd, SERVICE_UNAVAILABLE);
        return;
    }
    response_t res = kill (process_buffer->pid, SIGINT) ? INTERNAL_ERROR : OK;
    rsend (process_buffer->socket_fd, res);
    process_buffer->returned = res;
    process_buffer = NULL;
}

response_t process_handle (struct process_t* proc) {
    return system (proc->command) ? INTERNAL_ERROR : OK;
}

response_t process_kill (_pid_c pid_kill) {
    if (kill (pid_kill, SIGKILL) != 0) {
        return INTERNAL_ERROR;
    }
    return OK;
}

void chroot_main () {
    current_proc_id = (int*) mmap(NULL, sizeof (int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *current_proc_id = 0;
    
    sys_mnts = (struct system_mounts*) mmap(NULL, sizeof (struct system_mounts), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    struct mntent *ent;
    FILE *mnts_stream;

    mnts_stream = setmntent("/proc/mounts", "r");
    if (mnts_stream == NULL) {
        perror("setmntent");
        exit(1);
    }
    while ((ent = getmntent(mnts_stream)) != NULL) {
        memcpy (&sys_mnts->mounts[sys_mnts->mount_c], ent, sizeof (struct mntent));
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
    
    struct chroot_mount mounts[32] = {
        {"/proc", "proc", "proc", 0, -1},
        {"/sys", "sys", "", 1, -1},
        {"/dev", "dev", "", 1, -1},
        {*PORTAGE_DIR, *PORTDIR, *"", 0, -1},
    };
    
    memcpy (&chroot->mounts, &mounts, sizeof (struct chroot_mount) * 32);
    chroot->mount_c = 4;
    
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

mount_status mount_check (struct chroot_client* client, char* src, char* target) {
    char dest_temp[256];
    //sprintf (dest_temp, "%s/%s", new_root, dest);
    
    //normalize_path (dest_temp, dest_temp, strlen (dest_temp));
    /*
    int i;
    for (i=0; i!=sys_mnts->mount_c; i++) {
        if (strcmp (src, dest_temp) == 0) {
            return NO_MOUNT;
        }
        if (strcmp (target, sys_mnts->mounts[i].mnt_dir) == 0) {
            return IS_MOUNTED;
        }
    }*/
    
    return NOT_MOUNTED;
}

void chroot_mount (struct chroot_client* client) {
    char target[256];
    struct serve_client * buffer_client = &client->m_man->clients[client->sc_no];
    
    sprintf (target, "%s/%s", client->m_man->root, buffer_client->id);
    
    int i;
    for (i=0; i!=client->mount_c; i++) {
        
        if (strcmp(client->mounts[i].type, "") == 0) {
            ;
        }
    }
    
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

struct process_t* new_process (struct chroot_client* chroot, char* request, int sockfd) {
    chroot->proc_list[chroot->proc_c] = mmap(NULL, sizeof (struct process_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    struct process_t* n_proc = chroot->proc_list[chroot->proc_c];
    strcpy (n_proc->command, request);
    n_proc->proc_id = *current_proc_id;
    n_proc->parent = chroot;
    n_proc->pid = -1;
    n_proc->socket_fd = sockfd;
    n_proc->status = WAITING;
    
    *current_proc_id++;
    chroot->proc_c++;
    return n_proc;
}
