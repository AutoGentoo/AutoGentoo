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
#include <_string.h>
#include <response.h>

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

// client connection
void server_respond (int n, struct manager * m_man)
{
    char mesg[99999], *reqline[3], data_to_send[BYTES], path[99999];
    int rcvd, fd, bytes_read;
    char *ip;
    response_t res;

    memset((void*)mesg, (int)'\0', 99999);

    rcvd = recv(clients[n], mesg, 99999, 0);

    if (rcvd < 0) // receive error
        fprintf(stderr, ("recv() error\n"));
    else if (rcvd == 0) // receive socket closed
        fprintf(stderr, "Client disconnected upexpectedly.\n");
    else // message received
    {
        reqline[0] = strtok(mesg, " \t\n");
        ip = get_ip_from_fd (clients[n]);
        if (strncmp(reqline[0], "GET\0", 4) == 0) {
            reqline[1] = strtok(NULL, " \t");
            reqline[2] = strtok(NULL, " \t\n");
            if (strncmp(reqline[2], "HTTP/1.0", 8) != 0 && strncmp(reqline[2], "HTTP/1.1", 8) != 0) {
                rsend (clients[n], BAD_REQUEST);
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
                    sprintf (path, "%s/%s%s", m_man->root, m_man->clients[sc_no].hostname, reqline[1]);
                
                    if ((fd = open(path, O_RDONLY)) != -1) // FILE FOUND
                    {
                        rsend (clients[n], OK);
                        res = OK;
                        send (clients[n], "\n", 1, 0);
                        while ((bytes_read = read(fd, data_to_send, BYTES)) > 0)
                            write(clients[n], data_to_send, bytes_read);
                    }
                    else {
                        rsend (clients[n], NOT_FOUND);
                        res = NOT_FOUND;
                    }
                }
            }
        }
        else if (strncmp(reqline[0], "CMD\0", 4) == 0) {
            reqline[1] = strtok(NULL, " \t");
            reqline[2] = strtok(NULL, " \t\n");
            request_t rt = atoi (reqline[1]);
            
            res = exec_method (rt, m_man, reqline[2], clients[n]);
            
            rsend (clients[n], res);
        }
        else if (strncmp(reqline[0], "SRV\0", 4) == 0) {
            reqline[1] = strtok(NULL, " \t");
            reqline[2] = strtok(NULL, " \t\n");
            serve_c rt = atoi (reqline[1]);
            struct link_srv linked = get_link_srv (rt);
            char request_opts [linked.argc][1024];
            int sc_no;
            char sent = 0;
            
            int i;
            for (i=0; i != linked.argc; i++) {
                char *b = strtok (NULL, "\n");
                if (b!=NULL) {
                    strcpy(request_opts[i], b);
                }
                else {
                    rsend (clients[n], BAD_REQUEST);
                    res = BAD_REQUEST;
                    sent = 1;
                    break;
                }
            }
            if (sent == 0) {
                if (rt == CREATE) {
                    m_man->clients[m_man->client_c].ip_c = 0;
                    strcpy(m_man->clients[m_man->client_c].hostname, request_opts[0]);
                    strcpy(m_man->clients[m_man->client_c].ip[m_man->clients[m_man->client_c].ip_c], ip);
                    m_man->clients[m_man->client_c].ip_c++;
                    strcpy(m_man->clients[m_man->client_c].profile, request_opts[1]);
                    strcpy(m_man->clients[m_man->client_c].CHOST, request_opts[2]);
                    strcpy(m_man->clients[m_man->client_c].CFLAGS, request_opts[3]);
                    strcpy(m_man->clients[m_man->client_c].CXXFLAGS, "${CFLAGS}");
                    strcpy(m_man->clients[m_man->client_c].USE, request_opts[4]);
                    
                    strcpy(m_man->clients[m_man->client_c].PORTAGE_TMPDIR, "autogentoo/tmp");
                    strcpy(m_man->clients[m_man->client_c].PORTDIR, "/usr/portage");
                    strcpy(m_man->clients[m_man->client_c].DISTDIR, "/usr/portage/distfiles");
                    strcpy(m_man->clients[m_man->client_c].PKGDIR, "autogentoo/pkg");
                    strcpy(m_man->clients[m_man->client_c].PORT_LOGDIR, "autogentoo/log");
                    m_man->client_c++;
                    FILE * _fd = fopen (m_man->_config, "w+");
                    write_serve (fileno(_fd), m_man);
                    fclose (_fd);
                }
                else if (rt == ADDIP) {
                    sc_no = get_client_from_hostname (m_man, request_opts[0]);
                    if (sc_no == -1) {
                        rsend (clients[n], BAD_REQUEST);
                        res = BAD_REQUEST;
                        sent = 1;
                    }
                    else {
                        strcpy(m_man->clients[sc_no].ip[m_man->clients[sc_no].ip_c], ip);
                        m_man->clients[sc_no].ip_c++;
                    }
                }
                else if (rt == INIT) {
                    sc_no = get_client_from_ip (m_man, ip);
                    if (sc_no == -1) {
                        rsend (clients[n], FORBIDDEN);
                        res = FORBIDDEN;
                        sent = 1;
                    }
                    else {
                        init_serve_client (*m_man, m_man->clients[sc_no]);
                    }
                }
                else if (rt == GETCLIENT) {
                    sc_no = get_client_from_ip (m_man, ip);
                    sent = 1;
                    if (sc_no == -1) {
                        rsend (clients[n], NOT_FOUND);
                        res = NOT_FOUND;
                    }
                    else {
                        rsend (clients[n], OK);
                        res = OK;
                        char c_buff[256];
                        sprintf (c_buff, "%s\n%s\n%s\n%s\n", 
                                 m_man->clients[sc_no].CFLAGS, 
                                 m_man->clients[sc_no].CXXFLAGS, 
                                 m_man->clients[sc_no].CHOST,
                                 m_man->clients[sc_no].USE);
                        write (clients[n], c_buff, sizeof (c_buff));
                    }
                }
            }
            if (sent == 0) {
                rsend (clients[n], OK);
                res = OK;
            }
        }
        printf ("[%s](%s, %s): %d %s\n", ip, reqline[0], reqline[1], res.code, res.message);
    }
    
    
    // Closing SOCKET
    shutdown(
        clients[n],
        SHUT_RDWR); // All further send and recieve operations are DISABLED...
    close(clients[n]);
    clients[n] = -1;
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

    int i;
    for (i=0; i<CONNMAX; i++)
        clients[i]=-1;
    server_start("9490");
    
    printf ("Starting server\n");
    
    if (daemon) {
        daemonize (m_man->root);
    }
    
    while (1)
    {
        addrlen = sizeof(clientaddr);
        clients[slot] = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);
        
        if ((int)clients[slot] < 0) {
            error ("accept() error\n");
            continue;
        }
        
        server_respond(slot, m_man);
    
        while (clients[slot]!=-1) slot = (slot+1)%CONNMAX;
    }
}
