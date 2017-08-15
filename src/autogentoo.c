/*
 * autogentoo.c
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


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <serve_client.h>
#include <server.h>

struct server_config {
    char* ip; // Will always be this hosts ip
    char* port; // 9490
    char* root; // Same root as in serve_client_manager
};

struct AutoGentoo {
    struct serve_client_manager * manager;
    struct server_config cfg_server;
    int configfd;
}

struct AutoGentoo config_m;

void _getcwd (char* out, size_t size) {
    if (getcwd(out, size) == NULL)
       perror("getcwd() error");
}

int main (int argc, char ** argv) {
    char* b_root;
    char* b_fconfig;
    char  use_daemon = 0;
    
    while ((c = getopt (argc, argv, "r:c:d")) != -1) {
        switch (c) {
            case 'r':
                b_root = optarg;
                break;
            case 'c':
                b_fconfig = optarg;
                break;
            case 'd':
                use_daemon = 1;
                break;
            case '?':
                if (optopt == 'r' || optopt == 'c')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                fprintf (stderr,
                       "Unknown option character `\\x%x'.\n",
                       optopt);
                return 1;
            default:
                _getcwd (b_root, sizeof (b_root));
                b_fconfig = ".autogentoo.config";
        }
    }
    
    char b_hs [32];
    gethostname (b_hs, sizeof (b_hs));
    config_m = {
        init_manager (b_root),
        {
            get_ip_from_hostname (b_hs)
            "9490",
            b_root
        },
        open (b_fconfig, "a");
    }
    
    return 0;
}
