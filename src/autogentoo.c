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

void set_request (struct client_request * req, int type, char* atom) {
    if (req->type != -1) {
        printf ("Method %s can't be called with %s\n", request_names [req->type], request_names [type]);
        fflush(stdout);
        exit (1);
    }
    req->type = type;
    req->atom = atom;
}

struct _opts {
    char p[256];
    char f[64];
    unsigned d:1;
    unsigned s:1;
    unsigned c:1;
    request_t t;
    char atom[64];
};

int main (int argc, char ** argv) {
    struct _opts __opts = {"", ".autogentoo.config", 0,0,0,-1,""};
    _getcwd(__opts.p, sizeof __opts.p);
    
    struct client_request req;
    req.type = -1;
    int c;
    
    while ((c = getopt (argc, argv, "p:f:dhsci:I:r:R:")) != -1) {
        switch (c) {
            case 'p':
                strcpy(__opts.p, optarg);
                break;
            case 'f':
                strcpy(__opts.f, optarg);
                break;
            case 'd':
                __opts.d = 1;
                break;
            case 's':
                __opts.s = 1;
                break;
            case 'h':
                printf ("AutoGentoo help page\n\
This is the server binary\n\
Server Options (-s):\n\
  -p dir\tRoot directory, all clients are installed under this directory\n\
  -c file\tConfig writen to from by this program (loaded at boot)\n\
  -d\t\tFork program into daemon on boot\n\
  -h\t\tPrint this message and quit\n\
  -s\t\tEnable server mode\n\
\n\
Client Options\n\
  -c\t\tSetup /etc/portage/make.conf for portage binhost and flags\n\
  -i pkg\tInstall package on client\n\
  -I pkg\tInstall package only on server. use for\n\
  \t\tbuild dependencies only such as autotools\n\
  -r pkg\tRemove package from the client\n\
  -R pkg\tRemove package from both client and server\n\
");
                return 0;
                break;
            case 'c':
                __opts.c = 1;
                break;
            case 'i':
                strcpy(__opts.atom, optarg);
                __opts.t = install_c;
                set_request (&req, __opts.t, __opts.atom);
                break;
            case 'I':
                strcpy(__opts.atom, optarg);
                __opts.t = install_s;
                set_request (&req, __opts.t, __opts.atom);
                break;
            case 'r':
                strcpy (__opts.atom, optarg);
                __opts.t = remove_c;
                set_request (&req, __opts.t, __opts.atom);
                break;
            case 'R':
                strcpy (__opts.atom, optarg);
                __opts.t = remove_s;
                set_request (&req, __opts.t, __opts.atom);
                break;
            case '?':
                if (optopt == 'r' || optopt == 'f' || optopt == 'i' || optopt == 'I' || optopt == 'r' || optopt == 'R')
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
    
    if (__opts.s) {
        struct manager * m_man = malloc (sizeof (struct manager));
        strcpy(m_man->root, __opts.p);
        strcpy(m_man->_config, __opts.f);
        
        if (access (__opts.f, F_OK) != -1) {
            int _fd = open (__opts.f, O_RDONLY);
            read_serve (_fd, m_man);
            close (_fd);
        }
        
        strcpy(config_m.port, "9490");
        
        config_m.config_path = __opts.f;
        
        server_main (__opts.d, m_man);
    }
    else {
        // Client
        printf ("%s package %s\n", request_names[(int)req.type], req.atom);
    }
    return 0;
}
