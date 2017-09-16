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

    memset((void*)mesg, (int)'\0', 2048);

    // Create buffs to redirect STDOUT and STDERR
    int stdout_b, stderr_b;
    stdout_b = dup (STDOUT_FILENO);
    stderr_b = dup (STDERR_FILENO);
    dup2(clients[n], STDOUT_FILENO);
    dup2(clients[n], STDERR_FILENO);
    close(clients[n]);

    rcvd = recv(1, mesg, 2048, 0);
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
        reqline[2] = strtok(NULL, "\n");
        ip = get_ip_from_fd (1);
        if (reqline[2] == NULL) {
            res = BAD_REQUEST;
            rsend (1, BAD_REQUEST);
            reqline[0] = "\0"; // Make sure that the request doesn't continue
        }
        if (strncmp(reqline[0], "GET\0", 4) == 0) {
            if (strncmp(reqline[2], "HTTP/1.0", 8) != 0 && strncmp(reqline[2], "HTTP/1.1", 8) != 0) {
                rsend (1, BAD_REQUEST);
                res = BAD_REQUEST;
            }
            else {
                if (strncmp(reqline[1], "/\0", 2) == 0)
                    reqline[1] = "";

                char *ip = get_ip_from_fd (1);
                int sc_no = get_client_from_ip (m_man, ip);
                if (sc_no < 0) {
                    rsend (1, FORBIDDEN);
                    res = FORBIDDEN;
                }
                else {
                    sprintf (path, "%s/%s/autogentoo/pkg/%s", m_man->root, m_man->clients[sc_no].id, reqline[1]);
                    int fd, bytes_read, data_to_send;
                    if ((fd = open(path, O_RDONLY)) != -1) // FILE FOUND
                    {
                        rsend (1, OK);
                        res = OK;
                        send (1, "\n", 1, 0);
                        while ((bytes_read = read(fd, (void*)&data_to_send, sizeof(data_to_send))) > 0)
                            write(1, (void*)&data_to_send, bytes_read);
                    }
                    else {
                        rsend (1, NOT_FOUND);
                        res = NOT_FOUND;
                    }
                }
            }
        }
        else if (strncmp(reqline[0], "CMD\0", 4) == 0) {
            int exec_sock = dup(1);
            res = exec_method (reqline[1], m_man, reqline[2], ip, exec_sock);
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
            int sc_no;

            int i;
            for (i=0; i != (l_argc + linked.argc); i++) {
                request_opts[i] = strtok (NULL, "\n");
                if (request_opts[i] == NULL) {
                    rsend (1, BAD_REQUEST);
                    res = BAD_REQUEST;
                    sent = 1;
                    break;
                }
            }
            if (sent == 0) {
                if (rt == CREATE) {
                    strcpy(m_man->clients[m_man->client_c].hostname, request_opts[0]);
                    strcpy(m_man->clients[m_man->client_c].profile, request_opts[1]);
                    strcpy(m_man->clients[m_man->client_c].CHOST, request_opts[2]);
                    strcpy(m_man->clients[m_man->client_c].CFLAGS, request_opts[3]);
                    strcpy(m_man->clients[m_man->client_c].CXXFLAGS, "${CFLAGS}");
                    strcpy(m_man->clients[m_man->client_c].USE, request_opts[4]);
                    for (m_man->clients[m_man->client_c].extra_c=0; m_man->clients[m_man->client_c].extra_c!=(l_argc);m_man->clients[m_man->client_c].extra_c++) {
                        strcpy (m_man->clients[m_man->client_c].EXTRA[m_man->clients[m_man->client_c].extra_c], request_opts[m_man->clients[m_man->client_c].extra_c+5]);
                    }
                    strcpy(m_man->clients[m_man->client_c].PORTAGE_TMPDIR, "autogentoo/tmp");
                    strcpy(m_man->clients[m_man->client_c].PORTDIR, "/usr/portage");
                    strcpy(m_man->clients[m_man->client_c].DISTDIR, "/usr/portage/distfiles");
                    strcpy(m_man->clients[m_man->client_c].PKGDIR, "autogentoo/pkg");
                    strcpy(m_man->clients[m_man->client_c].PORT_LOGDIR, "autogentoo/log");
                    strcpy (m_man->clients[m_man->client_c].PORTAGE_DIR, "/usr/portage");
                    strcpy (m_man->clients[m_man->client_c].resolv_conf, "/etc/resolv.conf");
                    strcpy (m_man->clients[m_man->client_c].locale, "en_US.utf8");
                    
                    gen_id (m_man->clients[m_man->client_c].id, 14); // Leave extra space for buf and 1 for \0
                    
                    _ip_activate (m_man, ip, m_man->clients[m_man->client_c].id);
                    m_man->client_c++;
                    if(!m_man->debug) {
                        FILE * _fd = fopen (m_man->_config, "w+");
                        write_serve (fileno(_fd), m_man);
                        fclose (_fd);
                    }
                    write (1, m_man->clients[m_man->client_c - 1].id, strlen(m_man->clients[m_man->client_c - 1].id));
                    write (1, "\n", 1);
                    rsend (1, OK);
                    res = OK;
                    sent = 1;
                }
                else if (rt == ACTIVATE) {
                    sc_no = get_client_from_id (m_man, request_opts[0]);
                    if (sc_no == -1) {
                        rsend (1, BAD_REQUEST);
                        res = BAD_REQUEST;
                        sent = 1;
                    }
                    else {
                        _ip_activate (m_man, ip, m_man->clients[sc_no].id);
                    }
                    if(!m_man->debug) {
                        FILE * _fd = fopen (m_man->_config, "w+");
                        write_serve (fileno(_fd), m_man);
                        fclose (_fd);
                    }
                }
                else if (rt == INIT) {
                    sc_no = get_client_from_ip (m_man, ip);
                    if (sc_no == -1) {
                        rsend (1, FORBIDDEN);
                        res = FORBIDDEN;
                        sent = 1;
                    }
                    else {
                        init_serve_client (m_man, sc_no);
                    }
                }
                else if (rt == GETCLIENT) {
                    sc_no = get_client_from_id (m_man, request_opts[0]);
                    sent = 1;
                    if (sc_no == -1) {
                        rsend (1, NOT_FOUND);
                        res = NOT_FOUND;
                    }
                    else {
                        rsend (1, OK);
                        res = OK;
                        char c_buff[1024];
                        char EXTRA [2048];

                        sprintf (EXTRA, "");

                        int i_c;
                        for (i_c=0; i_c!=m_man->clients[sc_no].extra_c; i_c++) {
                            strcat (EXTRA, m_man->clients[sc_no].EXTRA[i_c]);
                            strcat (EXTRA, "\n");
                        }
                        sprintf (c_buff, "%d\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
                                 m_man->clients[sc_no].extra_c,
                                 m_man->clients[sc_no].CFLAGS,
                                 m_man->clients[sc_no].CXXFLAGS,
                                 m_man->clients[sc_no].CHOST,
                                 m_man->clients[sc_no].USE,
                                 m_man->clients[sc_no].hostname,
                                 m_man->clients[sc_no].profile,
                                 EXTRA);
                        write (1, c_buff, sizeof (c_buff));
                    }
                }
                else if (rt == STAGE1) {
                    sc_no = get_client_from_ip (m_man, ip);
                    if (sc_no == -1) {
                        rsend (1, FORBIDDEN);
                        res = FORBIDDEN;
                        sent = 1;
                    }
                    else {
                        char pkgs[8191];
                        FILE * fp = fopen ("/usr/portage/profiles/default/linux/packages.build", "r");
                        char line[255];
                        strcat (pkgs, "-u "); // Dont emerge packages that are already binaries
                        while (fgets(line, sizeof(line), fp) != NULL) {
                            char *pos;
                            if ((pos=strchr(line, '\n')) != NULL)
                                *pos = '\0';
                            if (line[0] == '#' || line[0] == '\n' || strcmp (line, "") == 0) {
                                continue;
                            }
                            strcat (pkgs, line);
                            strcat (pkgs, " ");
                        }
                        fclose (fp);
                        
                        int m_install_sock = dup(1);
                        res = __m_install (pkgs, m_man, sc_no, ip, m_install_sock);
                        m_man->clients[sc_no].state = STAGE3;
                        
                        m_man->clients[sc_no].chroot = chroot_new (m_man, i);
                        chroot_mount (m_man->clients[sc_no].chroot);
                        m_man->clients[sc_no].state = CHROOT;
                        
                        sent = 1;
                    }
                }
                else if (rt == UNOSYNC) {
                    sc_no = get_client_from_ip (m_man, ip);
                    if (sc_no > -1) {
                        
                        int m_install_sock = dup(1);
                        res = m_install ("-uDN @world", m_man, sc_no, ip, m_install_sock);
                        sent = 1;
                    }
                    else {
                        res = FORBIDDEN;
                        rsend (1, res); // Write to stdout instead of socket
                    }
                    sent = 1;
                }
                else if (rt == SYNC) {
                    if (system("emerge --sync") != 0) {
                        res = INTERNAL_ERROR;
                    }
                    else {
                        res = OK;
                    }

                    rsend (1, res); // Write to stdout instead of socket
                    sent = 1;
                }
                else if (rt == UPDATE) {
                    sc_no = get_client_from_ip (m_man, ip);
                    if (sc_no > -1) {
                        system ("emerge --sync");
                        int m_install_sock = dup(1);
                        res = m_install ("-uDN @world", m_man, sc_no, ip, m_install_sock);
                        sent = 1;
                    }
                    else {
                        res = FORBIDDEN;
                        rsend (1, res); // Write to stdout instead of socket
                    }
                    sent = 1;
                }
                else if (rt == EDIT) {
                    sc_no = get_client_from_id (m_man, request_opts[0]);
                    if (sc_no > -1) {
                        strcpy(m_man->clients[sc_no].hostname, request_opts[1]);
                        strcpy(m_man->clients[sc_no].profile, request_opts[2]);
                        strcpy(m_man->clients[sc_no].CHOST, request_opts[3]);
                        strcpy(m_man->clients[sc_no].CFLAGS, request_opts[4]);
                        strcpy(m_man->clients[sc_no].CXXFLAGS, "${CFLAGS}");
                        strcpy(m_man->clients[sc_no].USE, request_opts[5]);
                        for (m_man->clients[sc_no].extra_c=0; m_man->clients[sc_no].extra_c!=(l_argc);m_man->clients[sc_no].extra_c++) {
                            strcpy (m_man->clients[sc_no].EXTRA[m_man->clients[sc_no].extra_c], request_opts[m_man->clients[sc_no].extra_c+6]);
                        }
                        strcpy(m_man->clients[sc_no].PORTAGE_TMPDIR, "autogentoo/tmp");
                        strcpy(m_man->clients[sc_no].PORTDIR, "/usr/portage");
                        strcpy(m_man->clients[sc_no].DISTDIR, "/usr/portage/distfiles");
                        strcpy(m_man->clients[sc_no].PKGDIR, "autogentoo/pkg");
                        strcpy(m_man->clients[sc_no].PORT_LOGDIR, "autogentoo/log");
                        write_make_conf (*m_man, m_man->clients[sc_no]);

                        if(!m_man->debug) {
                            FILE * _fd = fopen (m_man->_config, "w+");
                            write_serve (fileno(_fd), m_man);
                            fclose (_fd);
                        }
                    }
                    else {
                        res = NOT_FOUND;
                        rsend (1, res); // Write to stdout instead of socket
                        sent = 1;
                    }
                }
                else if (rt == GETCLIENTS) {
                    char n_buff[12];
                    sprintf(n_buff, "%d", m_man->client_c);
                    write (1, n_buff, strlen(n_buff));
                    write (1, "\n", sizeof(char));
                    int i;
                    for (i=0; i!=m_man->client_c; i++) {
                        write (1, m_man->clients[i].id, 14);
                        write (1, "\n", sizeof(char));
                    } // No need to rsend, leave it for post scope
                }
                else if (rt == GETACTIVE) {
                    sc_no = get_client_from_ip (m_man, ip);
                    if (sc_no > -1) {
                        write (1, m_man->clients[sc_no].id, strlen(m_man->clients[sc_no].id));
                        write (1, "\n", 1);
                    }
                    else {
                        write (1, "invalid\n", 8);
                    }
                }
                else if (rt == GETSPEC) {
                    system ("lscpu > build.spec");
                    FILE *lspcu_fp = fopen("build.spec", "r");
                    char symbol;
                    if(lspcu_fp != NULL)
                    {
                        while((symbol = getc(lspcu_fp)) != EOF)
                        {
                            write (1, &symbol, sizeof (char));
                        }
                        fclose(lspcu_fp);
                        remove ("build.spec");
                    }
                }
                else if (rt == SCREMOVE) {
                    sc_no = get_client_from_id (m_man, request_opts[0]);
                    if (sc_no < 0) {
                        res = NOT_FOUND;
                        rsend (1, res);
                        sent = 1;
                    }
                    else {
                        remove_client (m_man, sc_no);
                        if(!m_man->debug) {
                            FILE * _fd = fopen (m_man->_config, "w+");
                            write_serve (fileno(_fd), m_man);
                            fclose (_fd);
                        }
                    }
                }
                else if (rt == REGEN) {
                    sc_no = get_client_from_ip (m_man, ip);
                    if (sc_no > -1) {
                        
                        int m_install_sock = dup (1);
                        res = m_install ("@preserved-rebuild", m_man, sc_no, ip, m_install_sock);
                        
                        sent = 1;
                    }
                    else {
                        res = FORBIDDEN;
                        rsend (1, res);
                        sent = 1;
                    }
                }
            }
            if (sent == 0) {
                rsend (1, OK);
                res = OK;
            }
        }
    }


    close (STDOUT_FILENO);
    close (STDERR_FILENO);
    dup2 (stdout_b, STDOUT_FILENO); // Restore stdout/stderr to terminal
    dup2 (stderr_b, STDERR_FILENO);

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
    
    chroot_main ();
    
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
