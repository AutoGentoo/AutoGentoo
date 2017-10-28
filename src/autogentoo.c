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
#include <sys/mman.h>
#include <chroot.h>
#include <serve_client.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <_string.h>

void _getcwd (char* out, size_t size) {
    if (getcwd(out, size) == NULL)
       perror("getcwd() error");
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
    unsigned c:2; // Bit field to 2 because we might as well complete the padding
    request_t t;
    char atom[64];
    char a[32];
};

int main (int argc, char ** argv) {
    struct _opts __opts = {"", ".autogentoo.config", 0,0,0,-1,""};
    _getcwd(__opts.p, sizeof __opts.p);

    struct client_request req;
    req.type = -1;
    int c;
    int a = 0; // Address specified

    while ((c = getopt (argc, argv, "p:f:dhsci:r:a:")) != -1) {
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
  -f file\tConfig writen to from by this program (loaded at boot)\n\
  -d\t\tFork program into daemon on boot\n\
  -h\t\tPrint this message and quit\n\
  -s\t\tEnable server mode\n\
\n\
Client Options\n\
  -c\t\tSetup /etc/portage/make.conf for portage binhost and flags\n\
  -i pkg\tInstall package (in the build chroot)\n\
  \t\tbuild dependencies only such as autotools\n\
  -r pkg\tRemove package (in the build chroot)\n\
  -a address\tConfigure the build server ip address\n\
\n\
All changes to the client must be made through emerge\n\
");
                return 0;
                break;
            case 'a':
                strcpy (__opts.a, optarg);
                a = 1;
                break;
            case 'c':
                __opts.c = 1;
                break;
            case 'i':
                strcpy(__opts.atom, optarg);
                __opts.t = install;
                set_request (&req, __opts.t, __opts.atom);
                break;
            case 'r':
                strcpy (__opts.atom, optarg);
                __opts.t = _remove;
                set_request (&req, __opts.t, __opts.atom);
                break;
            case '?':
                if (optopt == 'r' || optopt == 'f' || optopt == 'i'  || optopt == 'r' || optopt == 'a')
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
        struct manager * m_man = mmap(NULL, sizeof (struct manager), PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        strcpy(m_man->root, __opts.p);
        strcpy(m_man->_config, __opts.f);

        chdir (m_man->root);
        if (access (__opts.f, F_OK) != -1) {
            int _fd = open (__opts.f, O_RDONLY);
            read_serve (_fd, m_man);
            close (_fd);
        }

        server_main (__opts.d, m_man);
    }
    else {
        // Client
        char s_ip[32];
        struct passwd *pw = getpwuid(getuid());

        const char *homedir = pw->pw_dir;

        char configpath[256];
        char new_dir[256];
        sprintf (configpath, "%s/.config/autogentoo/ip.config", homedir);
        sprintf (new_dir, "%s/.config/autogentoo", homedir);

        _mkdir (new_dir);

        if (!a) { // Address not specified
            if (access (configpath, F_OK) != -1) {
                FILE* ip_c_fd = fopen (configpath, "r");
                fread (s_ip, sizeof (char), 32, ip_c_fd);
                fclose (ip_c_fd);
            }
            else {
                printf ("An address was not configured or specified\n");
                return 1;
            }
        }
        else {
            strcpy (s_ip, __opts.a);
            FILE* ip_c_fd = fopen (configpath, "w+");
            fwrite (s_ip, sizeof (char), 32, ip_c_fd);
            fclose (ip_c_fd);
            printf ("Saved ip address successfuly\n");
        }

        if ((int)req.type == -1) {
            printf ("Nothing to do. Exiting...\n");
            return 0;
        }

        char _hostname[100]; // To find the ip
        char _ip [16]; // current ip to define the range
        gethostname(_hostname, (size_t)100);
        hostname_to_ip(_hostname , _ip);

        printf ("%s package %s\n", request_names[(int)req.type], req.atom);
        if (strncmp (_ip, "127.0.0", 7) == 0) {
            printf ("Not connected to network, using localhost\n");
            fflush(stdout);
            strcpy (s_ip, "127.0.0.1");
        }


        response_t res;

        char message[256];
        res = ask_server (s_ip, req, &message[0]);

        printf ("%s\n", res.message);
    }
    return 0;
}
