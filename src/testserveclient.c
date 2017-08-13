/*
 * testserveclient.c
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
#include <serve_client.h>

int main(int argc, char **argv)
{
    struct serve_client_manager * manager;
    struct serve_client test;
    
    test.hostname = "test";
    test.profile = "default/linux/amd64/13.0";
    test.config = init_make_conf ("x86_64-pc-linux-gnu", "-march=native -O2 -pipe", "bindist mmx sse sse2");
    
    manager = init_manager ("/home/atuser/autogentoo");
    add_to_manager (manager, test);
    
    init_serve_client (manager, test);
    printf ("complete\n");
    fflush(stdout);
    return 0;
}

