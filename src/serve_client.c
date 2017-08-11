/*
 * serve_client.c
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


#include <serve_client.h>
#include <ip_convert.h>

struct serve_client * get_client_from_host (struct serve_client_manager * manager, char* hostname) {
    struct serve_client * current = manager->clients[0];
    while (current != NULL) {
        if (strcmp(current->hostname, hostname) == 0) {
            break;
        }
        current = current->next;
    }
    
    return current;
}

struct serve_client * get_client_from_ip (struct serve_client_manager * manager, char* ip) {
    char hostname [100];
    ip_to_hostname (ip, hostname);
    return get_client_from_host (manager, hostname);
}

void add_to_manager (struct serve_client_manager * manager, struct serve_client conf) {
    if (manager->used + 1 >= manager->size) {
        manager->clients = realloc (manager->clients, sizeof(struct serve_client*) * (manager->size + 5));
        manager->size += 5;
    }
    manager->clients[manager->used] = &conf;
    if (manager->used == 0) {
        manager->clients[manager->used]->back = NULL;
    }
    else {
        manager->clients[manager->used]->back = manager->clients[manager->used - 1]; // Update current object
        manager->clients[manager->used]->back->next = manager->clients[manager->used]; // Update the previous object
    }
    manager->clients[manager->used]->next = NULL; // Append so always last;
    manager->used++;
}

void init_serve_client (struct serve_client_manager * manager, struct serve_client conf);

struct serve_client_manager * init_manager (char * top_dir) {
    struct serve_client_manager * out = malloc (sizeof (struct serve_client_manager));
    strcpy(out->top_dir, top_dir);
    out->size = 5;
    out->used = 0;
    out->clients = malloc (sizeof(struct serve_client*) * 5);
    return out;
}
