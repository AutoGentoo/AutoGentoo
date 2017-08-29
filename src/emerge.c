/*
 * s_emerge.c
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


#include <emerge.h>

char * get_ip_from_fd (int fd) {
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int res = getpeername(fd, (struct sockaddr *)&addr, &addr_size);
    char *ip;
    ip = inet_ntoa(addr.sin_addr);
    return ip;
}

char** emergese (struct manager * m_man, struct serve_client client, char **command, size_t n) {
    char __root[128];
    char __croot[128];
    sprintf (__root, "--root='%s/%s'", m_man->root, client.id);
    sprintf (__croot, "--config-root='%s/%s/autogentoo'", m_man->root, client.id);
    char *out[32] = {"emerge", "-q", "--autounmask-coninue", "--buildpkg", "--usepkg", __root, __croot};
    int i;
    for (i=7; i!=(n+7); i++) {
        out[i] = command[i-7];
    }
    out[n+7] = NULL;
    return &out[0];
}

void emerges (struct manager * m_man, struct serve_client client, char* out) {
    sprintf (out, "emerge -q --autounmask-continue --buildpkg --usepkg --root='%s/%s' --config-root='%s/%s/autogentoo'", m_man->root, client.id, m_man->root, client.id);
}

void emergec (char *out) {
    sprintf (out, "emerge -q --autounmask-continue --usepkgonly");
}
