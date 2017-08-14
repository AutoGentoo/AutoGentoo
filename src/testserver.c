/*
 * testserver.c
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

int main(int argc, char **argv)
{
    int slot;
    struct sockaddr_in clientaddr;
    socklen_t addrlen;

    ROOT = "/home/atuser/";
    int i;
    
    for (i=0; i<CONNMAX; i++)
        clients[i]=-1;
    server_start("9490");
    
    // ACCEPT connections
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
    
    return 0;
}

