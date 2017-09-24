/*
 * server.c
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
#include <server.h>
#include <chroot.h>
#include <_string.h>
#include <response.h>
#include <sys/sysinfo.h>
#include <serve_client.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <srv_handle.h>

void server_start (char* port)
{
    struct addrinfo hints, *res, *p;

    // getaddrinfo for host
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, port, &hints, &res) != 0) {
        perror("getaddrinfo() error");
        exit(1);
    }
    // socket and bind
    for (p = res; p != NULL; p = p->ai_next) {
        listenfd = socket(p->ai_family, p->ai_socktype, 0);
        if (listenfd == -1)
            continue;
        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
            error("setsockopt(SO_REUSEADDR) failed");
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break;
    }
    if (p == NULL) {
        perror("socket() or bind()");
        exit(1);
    }

    freeaddrinfo(res);

    // listen for incoming connections
    if (listen(listenfd, 1000000) != 0) {
        perror("listen() error");
        exit(1);
    }
}

static int *hang_me;
static int *close_me;

void child_finished (int sig) {
    if (hang_me > 0) {
        waitpid (*hang_me, 0, WNOHANG);
        *hang_me = -1;
    }

    if (*close_me > 2) {
        close (*close_me);
    }
}

// client connection
void server_respond (int n, struct manager * m_man)
{
    char mesg[2048], *reqline[3], path[2048];
    int rcvd;

    char *ip;
    response_t res;
    
    int stdout_b, stderr_b;
    int b_client = clients[n];

    memset((void*)mesg, (int)'\0', 2048);
    
    rcvd = recv(clients[n], mesg, 2048, 0);
    int __error = 0;
    if (rcvd < 0) { // receive error
        fprintf(stderr, ("recv() error\n"));
        __error = 1;
    }
    else if (rcvd == 0) { // receive socket closed
        fprintf(stderr, "Client disconnected upexpectedly.\n");
        __error = 2;
    }
    else // message received
    {

        reqline[0] = strtok(mesg, " \t");
        reqline[1] = strtok(NULL, " \t");
        reqline[2] = strtok(NULL, "\r\n");
        ip = get_ip_from_fd (clients[n]);
        printf ("[%s](%s, %s)\n", ip, reqline[0], reqline[1]);
        
        // Create buffs to redirect STDOUT and STDERR
        stdout_b = dup (STDOUT_FILENO);
        stderr_b = dup (STDERR_FILENO);
        dup2(clients[n], STDOUT_FILENO);
        dup2(clients[n], STDERR_FILENO);
        close(clients[n]);
        clients[n] = 1;
        
        if (reqline[2] == NULL) {
            res = BAD_REQUEST;
            rsend (clients[n], BAD_REQUEST);
            reqline[0] = "\0"; // Make sure that the request doesn't continue
        }
        if (strncmp(reqline[0], "GET\0", 4) == 0) {
            if (strncmp(reqline[2], "HTTP/1.0", 8) != 0 && strncmp(reqline[2], "HTTP/1.1", 8) != 0) {
                rsend (clients[n], BAD_REQUEST);
                res = BAD_REQUEST;
            }
            else {
                if (strncmp(reqline[1], "/\0", 2) == 0)
                    reqline[1] = "";

                char *ip = get_ip_from_fd (clients[n]);
                int sc_no = get_client_from_ip (m_man, ip);
                if (sc_no < 0) {
                    rsend (clients[n], FORBIDDEN);
                    res = FORBIDDEN;
                }
                else {
                    sprintf (path, "%s/%s/autogentoo/pkg/%s", m_man->root, m_man->clients[sc_no].id, reqline[1]);
                    int fd, bytes_read, data_to_send;
                    if ((fd = open(path, O_RDONLY)) != -1) // FILE FOUND
                    {
                        rsend (clients[n], OK);
                        res = OK;
                        send (clients[n], "\n", 1, 0);
                        while ((bytes_read = read(fd, (void*)&data_to_send, sizeof(data_to_send))) > 0)
                            write(clients[n], (void*)&data_to_send, bytes_read);
                    }
                    else {
                        rsend (clients[n], NOT_FOUND);
                        res = NOT_FOUND;
                    }
                }
            }
        }
        else if (strncmp(reqline[0], "CMD\0", 4) == 0) {
            //int exec_sock = dup(clients[n]);
            res = exec_method (reqline[1], m_man, reqline[2], ip);
            rsend (clients[n], res);
        }
        else if (strncmp(reqline[0], "SRV\0", 4) == 0) {
            int l_argc = 0;
            if (strncmp (reqline[2], "HTTP", 4) != 0) {
                l_argc = atoi (reqline[2]);
            }
            struct link_srv linked = get_link_str (reqline[1]);
            serve_c rt = linked.command;
            char *request_opts[32];
            char sent = 0;
            
            int i;
            for (i=0; i != (l_argc + linked.argc); i++) {
                request_opts[i] = strtok (NULL, "\n");
                if (request_opts[i] == NULL) {
                    rsend (clients[n], BAD_REQUEST);
                    res = BAD_REQUEST;
                    sent = 1;
                    break;
                }
            }
            
            if (!sent) {
                res = srv_handle (clients[n], rt, m_man, request_opts, l_argc + linked.argc);
            }
        }
    }

    
    close (STDOUT_FILENO);
    close (STDERR_FILENO);
    dup2 (stdout_b, STDOUT_FILENO); // Restore stdout/stderr to terminal
    dup2 (stderr_b, STDERR_FILENO);
    
    clients[n] = b_client;
    
    shutdown(
        clients[n],
        SHUT_RDWR); // All further send and recieve operations are DISABLED...
        close(clients[n]);

    printf ("[%s](%s, %s): ", ip, reqline[0], reqline[1]);
    printf ("%d %s\n", res.code, res.message);
    fflush (stdout);
    *hang_me = getpid ();
    *close_me = clients[n];
}

void daemonize(char * _cwd)
{
    pid_t pid, sid;
    int fd;

    /* already a daemon */
    if ( getppid() == 1 ) return;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0)
    {
        exit(1);
    }

    if (pid > 0)
    {
        printf ("Forked to pid: %d\n", (int)pid);
        printf ("Moving to background\n");
        fflush(stdout);
        exit(0); /*Killing the Parent Process*/
    }

    /* At this point we are executing as the child process */

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0)
    {
        exit(1);
    }

    /* Change the current working directory. */
    if ((chdir(_cwd)) < 0)
    {
        exit(1);
    }


    fd = open("/dev/null",O_RDWR, 0);

    if (fd != -1)
    {
        dup2 (fd, STDIN_FILENO);
        dup2 (fd, STDOUT_FILENO);
        dup2 (fd, STDERR_FILENO);

        if (fd > 2)
        {
            close (fd);
        }
    }

    /*resettign File Creation Mask */
    umask(027);
}


void server_main (unsigned daemon, struct manager * m_man) {
    int slot;
    struct sockaddr_in clientaddr;
    socklen_t addrlen;
    
    hang_me = mmap(NULL, sizeof *hang_me, PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    close_me = mmap(NULL, sizeof *hang_me, PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    int i;
    for (i=0; i<CONNMAX; i++)
        clients[i]=-1;
    server_start("9490");

    printf ("Starting server\n");

    if (daemon) {
        daemonize (m_man->root);
    }

    signal(SIGCHLD, child_finished);

    while (1)
    {
        addrlen = sizeof(clientaddr);
        clients[slot] = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);

        if ((int)clients[slot] < 0) {
            error ("accept() error\n");
            continue;
        }
        
        pid_t res_pid;
        if ((res_pid = fork ()) == -1)
            exit (-1);
        else if (res_pid == 0) {
            server_respond(slot, m_man);
            _exit (0);
        }

        while (clients[slot]!=-1) slot = (slot+1)%CONNMAX;
    }
}
