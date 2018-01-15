//
// Created by Andrei Tumbar on 1/4/18.
//

#define _GNU_SOURCE
#include <kernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Kernel* kernel_new (Host* parent, char* target, char* version) {
    Kernel* out = malloc (sizeof(Kernel));
    out->parent = parent;
    out->kernel_target = strdup (target);
    out->version = strdup (version);
    
    out->kroot;
    asprintf (&out->kroot, "%s/%s/usr/src/linux-%s%s",
              parent->parent->location,
              parent->id,
              out->version,
              out->kernel_target
    );
    out->uri = NULL;
    
    return out;
}

void kernel_set_uri (Kernel* k, char* uri) {
    k->uri = strdup(uri);
}

void kernel_sync (Kernel* k) {
    if (k->uri == NULL) {
        lerror ("This kernel is not setup to sync!");
        return;
    }
    
    char* sync_command;
    asprintf (&sync_command, "cd %s && git pull", k->kroot);
    
    if (system(sync_command) != 0) {
        lerror ("Failed to sync kernel repo");
    }
    
    free (sync_command);
}

void kernel_free (Kernel* k) {
    free (k->version);
    free (k->uri);
    free (k->kroot);
    free (k->kernel_target);
    free (k);
}