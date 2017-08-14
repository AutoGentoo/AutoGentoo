/*
 * ip_scan.c
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

#include <ip_scan.h>

int findhosts (char ** ips) {
    // define range
    char hostname[100]; // To find the ip
    char ip [16]; // current ip to define the range
    gethostname(hostname, (size_t)100);
    hostname_to_ip(hostname , ip);
    char p_ip[4][4]; // Parsed ip into 8 bit ranges
    
    char* p;
    for (p = ip; p = strchr(p, '.'); ++p)
        *p = ' ';
    
    sscanf(ip,"%s %s %s %s",p_ip[0], p_ip[1], p_ip[2], p_ip[3]);
    
    int host_s = 0;
    int current_ip = 0;
    for (; current_ip <= 255; current_ip++) {
        char buf[16];
        sprintf (buf, "%s.%s.%s.%d", p_ip[0], p_ip[1], p_ip[2], current_ip);
        int stat = hostscan (buf);
        if (stat !=0) {
            sprintf(ips[host_s],"%s", buf);
            host_s++;
        }
    }
    
    return host_s;
}

int hostscan (char* hostname) {
    if (!checkip (hostname)) {
        return 0;
    }
    
    int portno = 9490;
    int valid = 0;
    while (portno <= 9490) {
        valid = checkvalid (hostname, portno);
        if (valid == 1) {
            return portno;
        }
        if (valid == -1) {
            return 0;
        }
        portno++;
    }
    return 0;
}

int checkip (char *ip) {
    char hostname[100];
    if (!ip_to_hostname (ip, hostname)) {
        return 0;
    }
    return 1;
}

int checkvalid (char* hostname, int portno) {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "ERROR opening socket");
    }

    server = gethostbyname(hostname);

    if (server == NULL) {
        return -1;
    }
    
    /* Set timeouts due to windows always connecting but not responding */
    struct timeval timeout;      
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
        fprintf(stderr, "setsockopt failed\n");

    if (setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
        fprintf(stderr, "setsockopt failed\n");
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        return 0; // Invalid port
    } else {
        return 1; // Valid port
    }

    close(sockfd);
}
