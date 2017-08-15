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
void server_respond (int n)
{
    char mesg[99999], *reqline[3], data_to_send[BYTES], path[99999];
    int rcvd, fd, bytes_read;

    memset((void*)mesg, (int)'\0', 99999);

    rcvd = recv(clients[n], mesg, 99999, 0);

    if (rcvd < 0) // receive error
        fprintf(stderr, ("recv() error\n"));
    else if (rcvd == 0) // receive socket closed
        fprintf(stderr, "Client disconnected upexpectedly.\n");
    else // message received
    {
        printf("%s", mesg);
        reqline[0] = strtok(mesg, " \t\n");
        if (strncmp(reqline[0], "GET\0", 4) == 0) {
            reqline[1] = strtok(NULL, " \t");
            reqline[2] = strtok(NULL, " \t\n");
            if (strncmp(reqline[2], "HTTP/1.0", 8) != 0 && strncmp(reqline[2], "HTTP/1.1", 8) != 0) {
                rsend (clients[n], BAD_REQUEST);
            }
            else {
                if (strncmp(reqline[1], "/\0", 2) == 0)
                    reqline[1] = "";

                strcpy(path, config_m.manager.top_dir);
                strcpy(&path[strlen(config_m.manager.top_dir)], reqline[1]);
                printf("file: %s\n", path);

                if ((fd = open(path, O_RDONLY)) != -1) // FILE FOUND
                {
                    rsend (clients[n], OK);
                    send (clients[n], "\n", 1, 0);
                    while ((bytes_read = read(fd, data_to_send, BYTES)) > 0)
                        write(clients[n], data_to_send, bytes_read);
                }
                else
                    rsend (clients[n], NOT_FOUND);
            }
        }
        else if (strncmp(reqline[0], "CMD\0", 4) == 0) {
            reqline[1] = strtok(NULL, " \t");
            reqline[2] = strtok(NULL, " \t\n");
            request_t rt = atoi (reqline[1]);
            
            response_t res = exec_method (rt, reqline[2], clients[n]);
            
            rsend (clients[n], res);
        }
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


void server_main (char daemon) {
    int slot;
    struct sockaddr_in clientaddr;
    socklen_t addrlen;

    int i;
    for (i=0; i<CONNMAX; i++)
        clients[i]=-1;
    server_start(config_m.port);
    
    if (daemon != 0) {
        // Create the daemon
        printf ("Creating daemon\n");
        daemonize (config_m.manager.top_dir);
    }
    
    printf ("Starting server\n");
    
    while (1)
    {
        addrlen = sizeof(clientaddr);
        clients[slot] = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);
    
        if (clients[slot]<0)
            error ("accept() error");
        else
        {
            if ( fork()==0 )
            {
                server_respond(slot);
                exit(0);
            }
        }
    
        while (clients[slot]!=-1) slot = (slot+1)%CONNMAX;
    }
}
