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
 
#include <autogentoo.h>

struct AutoGentoo config_m;

void _getcwd (char* out, size_t size) {
    if (getcwd(out, size) == NULL)
       perror("getcwd() error");
}

void print_bin (void * ptr, size_t size) {
    int i;
    for (i=0; i!=(int)size; i++) {
        unsigned char c = ((char*)ptr)[i] ;
        printf ("%02x ", c);
    }
}

int main (int argc, char ** argv) {
    char b_root[128];
    _getcwd (b_root, sizeof (b_root));
    char* b_fconfig = ".autogentoo.config";
    char  use_daemon = 0;
    int c;
    
    while ((c = getopt (argc, argv, "r:c:d")) != -1) {
        switch (c) {
            case 'r':
                strcpy(b_root, optarg);
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
                break;
        }
    }
    
    struct manager * m_man = malloc (sizeof (struct manager));
    strcpy(m_man->root, b_root);
    strcpy(m_man->_config, b_fconfig);
    
    if (access (b_fconfig, F_OK) != -1) {
        int _fd = open (b_fconfig, O_RDONLY);
        read_serve (_fd, m_man);
        close (_fd);
    }
    
    strcpy(config_m.port, "9490");
    
    config_m.config_path = b_fconfig;
    
    server_main (use_daemon, m_man);
    
    return 0;
}
