#include "writeconfig.h"
#include <stdlib.h>
#include "server.h"
#include "host.h"
#include <stdio.h>
#include <string.h>

void write_server (Server* server) {
    char* config_file_name = ".autogentoo.config";
    char* config_file = malloc (strlen (server->location) + strlen(config_file_name) + 2);
    sprintf (config_file, "%s/%s", server->location, config_file_name);
    
    FILE* to_write = fopen (config_file, "wb+");
    write_server_fp (server, to_write);
    
    fclose (to_write);
}

void write_server_fp (Server* server, FILE* fp) {
    write_string (server->location, fp);
    write_int (server->opts, fp);
    write_int (server->port, fp);
    
    write_int (server->hosts->n, fp);
    int i;
    for (i = 0; i != server->hosts->n; i++) {
        write_host_fp (*(Host**)vector_get (server->hosts, i), fp);
    }
    write_int (server->host_bindings->n, fp);
    for (i = 0; i != server->host_bindings->n; i++) {
        write_host_binding_fp ((HostBind*)vector_get (server->host_bindings, i), fp);
    }
}
void write_host_fp (Host* host, FILE* fp) {
    write_string (host->id, fp);
    write_int (host->status, fp);
    write_int (CHR_NOT_MOUNTED, fp); // What if system restarted (might as well assume unmounted)
    
    write_string (host->hostname, fp);
    write_string (host->profile, fp);
    write_string (host->cflags, fp);
    write_string (host->cxxflags, fp);
    write_string (host->use, fp);
    write_string (host->arch, fp);
    write_string (host->chost, fp);
    
    write_int (host->extra->n, fp);
    
    int i;
    for (i = 0; i != host->extra->n; i++) {
        write_string (string_vector_get (host->extra, i), fp);
    }
    
    write_string (host->portage_tmpdir, fp);
    write_string (host->portdir, fp);
    write_string (host->distdir, fp);
    write_string (host->pkgdir, fp);
    write_string (host->port_logdir, fp);
}
void write_host_binding_fp (HostBind* bind, FILE* fp) {
    write_string (bind->host->id, fp);
    write_string (bind->ip, fp);
}

Server* read_server (char* location) {
    char* config_file_name = ".autogentoo.config";
    char* config_file = malloc (strlen (location) + strlen(config_file_name) + 2);
    sprintf (config_file, "%s/%s", location, config_file_name);
    
    FILE* fp = fopen (config_file, "rb");
    if (fp == NULL) {
        return server_new (location, 9491, 0);
    }
    char* new_location = read_string (fp);
    int opts = read_int (fp);
    int port = read_int (fp);
    
    Server* out = server_new (new_location, port, opts);
    free (new_location);
    
    int n, i;
    n = read_int (fp);
    for (i = 0; i != n; i++) {
        Host* temp = read_host (fp);
        temp->parent = out;
        vector_add (out->hosts, &temp);
    }
    
    n = read_int (fp);
    for (i = 0; i != n; i++) {
        HostBind temp;
        read_host_binding (out, &temp, fp);
        vector_add (out->host_bindings, &temp);
    }
    
    fclose (fp);
    
    return out;
}

Host* read_host (FILE* fp) {
    Host* out = malloc (sizeof (Host));
    
    out->id = read_string (fp);
    out->status = read_int (fp);
    out->chroot_status = read_int (fp);
    
    out->hostname = read_string (fp);
    out->profile = read_string (fp);
    out->cflags = read_string (fp);
    out->cxxflags = read_string (fp);
    out->use = read_string (fp);
    out->arch = read_string (fp);
    out->chost = read_string (fp);
    
    out->extra = string_vector_new ();
    int n, i;
    n = read_int (fp);
    for (i = 0; i != n; i++) {
        char* temp = read_string (fp);
        string_vector_add (out->extra, temp);
        free (temp);
    }
    
    out->portage_tmpdir = read_string (fp);
    out->portdir = read_string (fp);
    out->distdir = read_string (fp);
    out->pkgdir = read_string (fp);
    out->port_logdir = read_string (fp);
    
    return out;
}

void read_host_binding (Server* server, HostBind* dest, FILE* fp) {
    char* _host_id = read_string (fp);
    dest->host = server_host_search(server, _host_id);
    dest->ip = read_string (fp);
    free (_host_id);
}

void write_string (char* src, FILE* fp) {
    fputs (src, fp);
    fputc (0, fp);
}

char* read_string (FILE* fp) {
    char c;
    int i;
    for (i = 0, c = fgetc (fp); c != 0; i++, c = fgetc(fp));
    fseek (fp, -i - 1, SEEK_CUR);
    char* out = malloc (i + 1);
    fread (out, 1, i + 1, fp);
    
    return out;
}

void write_int (int src, FILE* fp) {
    fwrite (&src, sizeof(int), 1, fp);
}

int read_int (FILE* fp) {
    int out;
    fread (&out, sizeof(int), 1, fp);
    return out;
}