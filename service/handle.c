#include <stdio.h>
#include <stdlib.h>
#include "server.h"
#include "handle.h"
#include "chroot.h"
#include "host.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stage.h>

RequestLink requests[] = {
        {"GET",            GET},
        {"INSTALL",        INSTALL},
        {"CMD INSTALL",    INSTALL},
        //{"SRV DEVCREATE", SRV_DEVCREATE},
        {"SRV EDIT",       SRV_EDIT},
        {"SRV ACTIVATE",   SRV_ACTIVATE},
        {"SRV HOSTREMOVE", SRV_HOSTREMOVE},
        {"SRV MNTCHROOT",  SRV_MNTCHROOT},
        {"SRV GETHOSTS",   SRV_GETHOSTS},
        {"SRV GETHOST",    SRV_GETHOST},
        {"SRV GETACTIVE",  SRV_GETACTIVE},
        {"SRV GETSPEC",    SRV_GETSPEC}
};

SHFP parse_request(char* parse_line, char** args) {
    int i;
    for (i = 0; i != sizeof(requests) / sizeof(RequestLink); i++) {
        size_t current_length = strlen(requests[i].request_ident);
        if (strncmp(parse_line, requests[i].request_ident, current_length) == 0) {
            char* temp = parse_line + current_length;
            int j;
            for (j = 0, args[j] = strtok(temp, " \t\n"); args[j] != NULL; j++, args[j] = strtok(NULL, " \t\n"));
            return requests[i].call;
        }
    }
    
    return NULL;
}

response_t GET(Connection* conn, char** args, int start) {
    response_t res;
    if (strcmp(args[1], "HTTP/1.0") != 0 && strcmp(args[1], "HTTP/1.1") != 0) {
        rsend(conn, BAD_REQUEST);
        res = BAD_REQUEST;
        res.len = 0;
        return res;
    }
    
    if (conn->bounded_host == NULL) {
        rsend(conn, FORBIDDEN);
        res = FORBIDDEN;
        res.len = 0;
        return res;
    }
    
    char path[256];
    
    sprintf (path, "%s/%s/%s/%s", conn->parent->location, conn->bounded_host->id, conn->bounded_host->pkgdir, args[0]);
    int fd, data_to_send;
    ssize_t bytes_read;
    if ((fd = open(path, O_RDONLY)) != -1) // FILE FOUND
    {
        rsend(conn, OK);
        res = OK;
        write(conn->fd, "\n", 1);
        while ((bytes_read = read(fd, (void*) &data_to_send, sizeof(data_to_send))) > 0)
            write(conn->fd, (void*) &data_to_send, (size_t) bytes_read);
    } else {
        rsend(conn, NOT_FOUND);
        res = NOT_FOUND;
    }
    
    res.len = 0;
    return res;
}

response_t INSTALL(Connection* conn, char** args, int start) {
    if (conn->bounded_host == NULL) {
        rsend(conn, FORBIDDEN);
        return FORBIDDEN;
    }
    
    if ((conn->request + start)[0] == 0) {
        return BAD_REQUEST;
    }
    
    response_t res = host_install(conn->bounded_host, conn->request + start);
    
    return res;
}

/* SRV Configure requests */
int prv_host_edit(Host* host, int argc, StringVector* data) {
    enum {
        HOSTNAME,
        PROFILE,
        CHOST,
        CFLAGS,
        USE
    };
    
    if (data->n < USE + 1 + argc) {
        return 1;
    }
    
    string_overwrite(&host->hostname, string_vector_get(data, HOSTNAME), 1);
    string_overwrite(&host->profile, string_vector_get(data, PROFILE), 1);
    string_overwrite(&host->cflags, string_vector_get(data, CFLAGS), 1);
    string_overwrite(&host->chost, string_vector_get(data, CHOST), 1);
    string_overwrite(&host->cxxflags, "${CFLAGS}", 1);
    string_overwrite(&host->use, string_vector_get(data, USE), 1);
    
    if (host->extra != NULL) {
        string_vector_free(host->extra);
    }
    
    host->extra = string_vector_new();
    
    int i;
    for (i = USE + 1; i != data->n; i++) {
        string_vector_add(host->extra, string_vector_get(data, i));
    }
    
    string_overwrite(&host->portage_tmpdir, "/autogentoo/tmp", 1);
    string_overwrite(&host->portdir, "/usr/portage", 1);
    string_overwrite(&host->distdir, "/usr/portage/distfiles", 1);
    string_overwrite(&host->pkgdir, "/autogentoo/pkg", 1);
    string_overwrite(&host->port_logdir, "/autogentoo/log", 1);
    
    return 0;
}

Host* prv_host_new(Server* server, int argc, StringVector* data) {
    Host* host = host_new(server, host_id_new());
    if (prv_host_edit(host, argc, data) == 1) {
        free(host->id);
        free(host);
        return NULL;
    }
    return host;
}

response_t SRV_CREATE(Connection* conn, char** args, int start) {
    int argc = 0;
    if (strncmp(args[0], "HTTP", 4) != 0) {
        char* end;
        argc = (int) strtol(args[0], &end, 10);
    }
    
    char* request = conn->request + start;
    
    StringVector* data = string_vector_new();
    string_vector_split(data, request, "\n");
    
    Host* new_host = prv_host_new(conn->parent, argc, data);
    
    if (new_host == NULL) {
        return BAD_REQUEST;
    }
    
    // Added the host and bind it
    vector_add(conn->parent->hosts, &new_host);
    server_bind(conn, new_host);
    
    write(conn->fd, new_host->id, strlen(new_host->id));
    write(conn->fd, "\n", 1);
    
    string_vector_free(data);
    
    return OK;
}

response_t SRV_EDIT(Connection* conn, char** args, int start) {
    int argc = 0;
    if (strncmp(args[0], "HTTP", 4) != 0) {
        char* end;
        argc = (int) strtol(args[0], &end, 10);
    }
    
    char id_to_edit[16];
    unsigned long i_split = strchr(conn->request + start, '\n') - (conn->request + start);
    strncpy (id_to_edit, conn->request + start, i_split);
    
    char* request = conn->request + start + i_split + 1;
    StringVector* data = string_vector_new();
    string_vector_split(data, request, "\n");
    
    if (prv_host_edit(conn->bounded_host, argc, data)) {
        return BAD_REQUEST;
    }
    
    string_vector_free(data);
    
    return OK;
}

response_t SRV_ACTIVATE(Connection* conn, char** args, int start) {
    Host* found = server_host_search(conn->parent, args[0]);
    
    if (found == NULL) {
        return NOT_FOUND;
    }
    
    server_bind(conn, found);
    
    return OK;
}

response_t SRV_HOSTREMOVE(Connection* conn, char** args, int start) {
    int i;
    
    // Remove the binding
    for (i = 0; i != conn->parent->host_bindings->n; i++) {
        Host** tmp = (Host**) (((void***) vector_get(conn->parent->host_bindings, i))[1]);
        if (strcmp((*tmp)->id, args[0]) == 0) {
            vector_remove(conn->parent->host_bindings, i);
            // dont break because multiple clients can point to the same host
        }
    }
    
    // Remove the definition
    for (i = 0; i != conn->parent->hosts->n; i++) {
        if (strcmp((*(Host**) vector_get(conn->parent->host_bindings, i))->id, args[0]) == 0) {
            vector_remove(conn->parent->host_bindings, i);
            break; // Two hosts cant have the same id (at least they are not support to...)
        }
    }
    
    return OK;
}

/* SRV Utility request */

response_t SRV_MNTCHROOT(Connection* conn, char** args, int start) {
    if (conn->bounded_host == NULL) {
        return FORBIDDEN;
    }
    
    return chroot_mount(conn->bounded_host);
}

void prv_fd_write_str(int fd, char* str) {
    if (str == NULL) {
        return;
    }
    write(fd, str, strlen(str));
    write(fd, "\n", 1);
}

/* SRV Metadata requests */
response_t SRV_GETHOST(Connection* conn, char** args, int start) {
    Host* host = server_host_search(conn->parent, args[0]);
    
    if (host == NULL) {
        return NOT_FOUND;
    }
    
    if (host->extra != NULL) {
        char t[8];
        sprintf (t, "%d", host->extra->n);
        prv_fd_write_str(conn->fd, t);
        
    }
    prv_fd_write_str(conn->fd, host->cflags);
    prv_fd_write_str(conn->fd, host->cxxflags);
    prv_fd_write_str(conn->fd, host->chost);
    prv_fd_write_str(conn->fd, host->use);
    prv_fd_write_str(conn->fd, host->hostname);
    prv_fd_write_str(conn->fd, host->profile);
    
    if (host->extra != NULL) {
        int i;
        for (i = 0; i != host->extra->n; i++) {
            char* current_str = string_vector_get(host->extra, i);
            write(conn->fd, current_str, strlen(current_str));
            write(conn->fd, "\n", 1);
        }
    }
    
    return OK;
}

response_t SRV_GETHOSTS(Connection* conn, char** args, int start) {
    char t[8];
    sprintf (t, "%d\n", conn->parent->hosts->n);
    write(conn->fd, t, strlen(t));
    
    int i;
    for (i = 0; i != conn->parent->hosts->n; i++) {
        char* temp = (*(Host**) vector_get(conn->parent->hosts, i))->id;
        write(conn->fd, temp, strlen(temp));
        write(conn->fd, "\n", 1);
    }
    
    return OK;
}

response_t SRV_GETACTIVE(Connection* conn, char** args, int start) {
    if (conn->bounded_host == NULL) {
        char* out = "invalid\n";
        write(conn->fd, out, strlen(out));
        return OK;
    }
    
    write(conn->fd, conn->bounded_host->id, strlen(conn->bounded_host->id));
    write(conn->fd, "\n", 1);
    
    return OK;
}

response_t SRV_GETSPEC(Connection* conn, char** args, int start) {
    system("lscpu > build.spec");
    FILE* lspcu_fp = fopen("build.spec", "r");
    int symbol;
    if (lspcu_fp != NULL) {
        while ((symbol = getc(lspcu_fp)) != EOF) {
            write(conn->fd, &symbol, sizeof(char));
        }
        fclose(lspcu_fp);
    }
    remove("build.spec");
    
    return OK;
}

response_t SRV_GETTEMPPLATES(Connection* conn, char** args, int start) {
    StringVector* templates = host_template_get_all();
    
    write(conn->fd, &templates->n, sizeof(int));
    
    int i;
    char* b;
    for (i = 0; i != templates->n; i++) {
        write(conn->fd, "\n", 1);
        b = string_vector_get(templates, i);
        write(conn->fd, b, strlen(b));
        
    }
    write(conn->fd, "\n", 1);
    return OK;
}