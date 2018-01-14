#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include "server.h"
#include "handle.h"
#include "chroot.h"
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stage.h>
#include <writeconfig.h>
#include <response.h>

RequestLink requests[] = {
        {"GET",              GET},
        {"INSTALL",          INSTALL},
        {"CMD INSTALL",      INSTALL}, // Alias for INSTALL (this was the old usage)
        {"SRV EDIT",         SRV_EDIT},
        {"SRV ACTIVATE",     SRV_ACTIVATE},
        {"SRV HOSTREMOVE",   SRV_HOSTREMOVE},
        {"SRV MNTCHROOT",    SRV_MNTCHROOT},
        {"SRV GETHOSTS",     SRV_GETHOSTS},
        {"SRV GETHOST",      SRV_GETHOST},
        {"SRV GETACTIVE",    SRV_GETACTIVE},
        {"SRV GETSPEC",      SRV_GETSPEC},
        {"SRV GETTEMPLATES", SRV_GETTEMPLATES},
        {"SRV TEMPLATE NEW", SRV_TEMPLATE_NEW},
        {"SRV TEMPLATE_NEW", SRV_TEMPLATE_NEW}, // Alias for the previous one
        {"SRV TEMPLATE",     SRV_TEMPLATE},
        {"SRV GETSTAGED",    SRV_GETSTAGED},
        {"SRV GETSTAGE",     SRV_GETSTAGE},
        {"SRV HANDOFF",      SRV_HANDOFF},
        {"SRV SAVE",         SRV_SAVE},
        {"SRV HOSTWRITE",    HOSTWRITE},
        
        /* Binary requests */
        {"BIN SERVER",       BIN_SERVER},
        
        /* General */
        {"EXIT",             EXIT}
};

SHFP parse_request (char* parse_line, StringVector* args) {
    int i;
    for (i = 0; i != sizeof (requests) / sizeof (RequestLink); i++) {
        size_t current_length = strlen (requests[i].request_ident);
        if (strncmp (parse_line, requests[i].request_ident, current_length) == 0) {
            string_vector_split (args, parse_line + current_length, " \t\n");
            return requests[i].call;
        }
    }
    
    return NULL;
}

response_t GET (Connection* conn, char** args, int start, int argc) {
    response_t res;
    
    if (conn->bounded_host == NULL)
        return FORBIDDEN;
    
    if (strcmp (args[1], "HTTP/1.0") != 0 && strcmp (args[1], "HTTP/1.1") != 0)
        return BAD_REQUEST;
    
    char* path;
    asprintf (&path, "%s/%s/%s/%s", conn->parent->location, conn->bounded_host->id, conn->bounded_host->pkgdir, args[0]);
    
    int fd, data_to_send;
    
    if ((fd = open (path, O_RDONLY)) != -1) // FILE FOUND
    {
        ssize_t bytes_read;
        rsend (conn, OK);
        res = OK;
        write (conn->fd, "\n", 1);
        while ((bytes_read = read (fd, (void*)&data_to_send, sizeof (data_to_send))) > 0)
            write (conn->fd, (void*)&data_to_send, (size_t)bytes_read);
        close (fd);
    }
    else {
        rsend (conn, NOT_FOUND);
        res = NOT_FOUND;
    }
    
    free (path);
    res.len = 0;
    return res;
}

response_t INSTALL (Connection* conn, char** args, int start, int argc) {
    if (conn->bounded_host == NULL) {
        rsend (conn, FORBIDDEN);
        return FORBIDDEN;
    }
    
    if ((conn->request + start)[0] == 0) {
        return BAD_REQUEST;
    }
    
    response_t res = host_install (conn->bounded_host, conn->request + start);
    
    return res;
}

/* SRV Configure requests */
int prv_host_edit (Host* host, int argc, StringVector* data) {
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
    
    string_overwrite (&host->hostname, string_vector_get (data, HOSTNAME), 1);
    string_overwrite (&host->profile, string_vector_get (data, PROFILE), 1);
    string_overwrite (&host->cflags, string_vector_get (data, CFLAGS), 1);
    string_overwrite (&host->chost, string_vector_get (data, CHOST), 1);
    string_overwrite (&host->cxxflags, "${CFLAGS}", 1);
    string_overwrite (&host->use, string_vector_get (data, USE), 1);
    
    if (host->extra != NULL) {
        string_vector_free (host->extra);
    }
    
    host->extra = string_vector_new ();
    
    int i;
    for (i = USE + 1; i != data->n; i++) {
        string_vector_add (host->extra, string_vector_get (data, i));
    }
    
    string_overwrite (&host->portage_tmpdir, "/autogentoo/tmp", 1);
    string_overwrite (&host->portdir, "/usr/portage", 1);
    string_overwrite (&host->distdir, "/usr/portage/distfiles", 1);
    string_overwrite (&host->pkgdir, "/autogentoo/pkg", 1);
    string_overwrite (&host->port_logdir, "/autogentoo/log", 1);
    
    return 0;
}

Host* prv_host_new (Server* server, int argc, StringVector* data) {
    Host* host = host_new (server, host_id_new ());
    if (prv_host_edit (host, argc, data) == 1) {
        free (host->id);
        free (host);
        return NULL;
    }
    return host;
}

response_t SRV_CREATE (Connection* conn, char** args, int start) {
    int argc = 0;
    if (strncmp (args[0], "HTTP", 4) != 0) {
        char* end;
        argc = (int)strtol (args[0], &end, 10);
    }
    
    char* request = conn->request + start;
    
    StringVector* data = string_vector_new ();
    string_vector_split (data, request, "\n");
    
    Host* new_host = prv_host_new (conn->parent, argc, data);
    
    if (new_host == NULL) {
        return BAD_REQUEST;
    }
    
    // Added the host and bind it
    vector_add (conn->parent->hosts, &new_host);
    server_bind (conn, new_host);
    
    write (conn->fd, new_host->id, strlen (new_host->id));
    write (conn->fd, "\n", 1);
    
    string_vector_free (data);
    
    return OK;
}

response_t SRV_EDIT (Connection* conn, char** args, int start, int argc) {
    int e_argc = 0;
    if (strncmp (args[0], "HTTP", 4) != 0) {
        char* end;
        e_argc = (int)strtol (args[0], &end, 10);
    }
    
    char id_to_edit[16];
    unsigned long i_split = strchr (conn->request + start, '\n') - (conn->request + start);
    strncpy (id_to_edit, conn->request + start, i_split);
    
    char* request = conn->request + start + i_split + 1;
    StringVector* data = string_vector_new ();
    string_vector_split (data, request, "\n");
    
    if (prv_host_edit (conn->bounded_host, e_argc, data)) {
        return BAD_REQUEST;
    }
    
    string_vector_free (data);
    
    return OK;
}

response_t SRV_ACTIVATE (Connection* conn, char** args, int start, int argc) {
    Host* found = server_host_search (conn->parent, args[0]);
    
    if (found == NULL) {
        return NOT_FOUND;
    }
    
    server_bind (conn, found);
    
    return OK;
}

response_t SRV_HOSTREMOVE (Connection* conn, char** args, int start, int argc) {
    int i;
    
    // Remove the binding
    for (i = 0; i != conn->parent->host_bindings->n; i++) {
        Host** tmp = (Host**)(((void***)vector_get (conn->parent->host_bindings, i))[1]);
        if (strcmp ((*tmp)->id, args[0]) == 0) {
            vector_remove (conn->parent->host_bindings, i);
            // dont break because multiple clients can point to the same host
        }
    }
    
    // Remove the definition
    for (i = 0; i != conn->parent->hosts->n; i++) {
        if (strcmp ((*(Host**)vector_get (conn->parent->host_bindings, i))->id, args[0]) == 0) {
            vector_remove (conn->parent->host_bindings, i);
            break; // Two hosts cant have the same id (at least they are not support to...)
        }
    }
    
    return OK;
}

/* SRV Utility request */

response_t SRV_MNTCHROOT (Connection* conn, char** args, int start, int argc) {
    if (conn->bounded_host == NULL) {
        return FORBIDDEN;
    }
    
    return chroot_mount (conn->bounded_host);
}

void prv_fd_write_str (int fd, char* str) {
    if (str == NULL) {
        return;
    }
    write (fd, str, strlen (str));
    write (fd, "\n", 1);
}

/* SRV Metadata requests */
response_t SRV_GETHOST (Connection* conn, char** args, int start, int argc) {
    Host* host = server_host_search (conn->parent, args[0]);
    
    if (host == NULL) {
        return NOT_FOUND;
    }
    
    if (host->extra != NULL) {
        char t[8];
        sprintf (t, "%d", (int)host->extra->n);
        prv_fd_write_str (conn->fd, t);
        
    }
    prv_fd_write_str (conn->fd, host->cflags);
    prv_fd_write_str (conn->fd, host->cxxflags);
    prv_fd_write_str (conn->fd, host->chost);
    prv_fd_write_str (conn->fd, host->use);
    prv_fd_write_str (conn->fd, host->hostname);
    prv_fd_write_str (conn->fd, host->profile);
    
    if (host->extra != NULL) {
        int i;
        for (i = 0; i != host->extra->n; i++) {
            char* current_str = string_vector_get (host->extra, i);
            write (conn->fd, current_str, strlen (current_str));
            write (conn->fd, "\n", 1);
        }
    }
    
    return OK;
}

response_t SRV_GETHOSTS (Connection* conn, char** args, int start, int argc) {
    char t[8];
    sprintf (t, "%d\n", (int)conn->parent->hosts->n);
    write (conn->fd, t, strlen (t));
    
    int i;
    for (i = 0; i != conn->parent->hosts->n; i++) {
        char* temp = (*(Host**)vector_get (conn->parent->hosts, i))->id;
        write (conn->fd, temp, strlen (temp));
        write (conn->fd, "\n", 1);
    }
    
    return OK;
}

response_t SRV_GETACTIVE (Connection* conn, char** args, int start, int argc) {
    if (conn->bounded_host == NULL) {
        char* out = "invalid\n";
        write (conn->fd, out, strlen (out));
        return OK;
    }
    
    write (conn->fd, conn->bounded_host->id, strlen (conn->bounded_host->id));
    write (conn->fd, "\n", 1);
    
    return OK;
}

response_t SRV_GETSPEC (Connection* conn, char** args, int start, int argc) {
    system ("lscpu > build.spec");
    FILE* lspcu_fp = fopen ("build.spec", "r");
    int symbol;
    if (lspcu_fp != NULL) {
        while ((symbol = getc (lspcu_fp)) != EOF) {
            write (conn->fd, &symbol, sizeof (char));
        }
        fclose (lspcu_fp);
    }
    remove ("build.spec");
    
    return OK;
}

response_t SRV_GETTEMPLATES (Connection* conn, char** args, int start, int argc) {
    StringVector* templates = host_template_get_all ();
    
    char __n[16];
    sprintf(__n, "%d", (int)templates->n);
    write (conn->fd, &__n, strlen (__n));
    
    int i;
    for (i = 0; i != templates->n; i++) {
        char* b;
        write (conn->fd, "\n", 1);
        b = string_vector_get (templates, i);
        write (conn->fd, b, strlen (b));
        
    }
    write (conn->fd, "\n", 1);
    return OK;
}

response_t SRV_TEMPLATE_NEW (Connection* conn, char** args, int start, int argc) {
    /* We dont need to bind a template 
     * because it doesn't need to 
     * auto-detect destination directory for GET
     */
    
    HostTemplate* t = host_template_new (conn->parent, args[0]);
    
    small_map_insert (t->parent->stages, t->new_id, t);
    
    write (conn->fd, t->new_id, strlen (t->new_id));
    write (conn->fd, "\n", 1);
    return OK;
}

response_t SRV_TEMPLATE (Connection* conn, char** args, int start, int argc) {
    HostTemplate* t = small_map_get (conn->parent->stages, args[0]);
    if (t == NULL)
        return NOT_FOUND;
    
    StringVector* command_entries = string_vector_new ();
    string_vector_split (command_entries, conn->request + start, " \t\n");
    
    char* fname = NULL;
    int i;
    response_t ret = OK;
    for (i = 0; i != command_entries->n; i++) {
        char* current_command = string_vector_get (command_entries, i);
        if (strcmp (current_command, "DOWNLOAD") == 0) {
            fname = host_template_download (t);
            if (fname == NULL) {
                return INTERNAL_ERROR;
            }
        }
        else if (strcmp (current_command, "EXTRACT") == 0) {
            if (fname == NULL) {
                fname = host_template_download (t);
            }
            
            ret = host_template_extract (t, fname);
            if (ret.code != 200) {
                return ret;
            }
        }
        else if (strcmp (current_command, "ALL") == 0) {
            ret = host_template_stage (t);
        }
        else {
            fname = current_command;
        }
    }
    
    return ret;
}

response_t SRV_GETSTAGED (Connection* conn, char** args, int start, int argc) {
    char __n[16];
    sprintf(__n, "%d", (int)conn->parent->stages->n);
    write (conn->fd, &__n, strlen (__n));
    write (conn->fd, "\n", 1);
    
    int i;
    for (i = 0; i != conn->parent->stages->n; i++) {
        HostTemplate* __t = (*(HostTemplate***)vector_get (conn->parent->stages, i))[1];
        
        write (conn->fd, __t->new_id, strlen (__t->new_id));
        write (conn->fd, "\n", 1);
    }
    
    return OK;
}

response_t SRV_GETSTAGE (Connection* conn, char** args, int start, int argc) {
    char* buf;
    HostTemplate* __t = small_map_get (conn->parent->stages, args[0]);
    
    asprintf (&buf, "%s\n%s\n%s\n%s\n%d\n",
              __t->new_id,
              __t->id,
              __t->cflags,
              __t->chost,
              __t->extra_c
    );
    
    write (conn->fd, buf, strlen (buf));
    free (buf);
    
    int j;
    for (j = 0; j != __t->extra_c; j++) {
        asprintf (&buf, "%s %d\n", __t->extras[j].make_extra, __t->extras[j].select);
        write (conn->fd, buf, strlen (buf));
        free (buf);
    }
    
    return OK;
}

response_t SRV_HANDOFF (Connection* conn, char** args, int start, int argc) {
    HostTemplate* __t = small_map_get (conn->parent->stages, args[0]);
    if (!__t)
        return NOT_FOUND;
    
    Host* new_host = host_template_handoff (__t);
    if (!new_host)
        return INTERNAL_ERROR;
    
    vector_add (conn->parent->hosts, &new_host);
    
    return OK;
}

response_t SRV_SAVE (Connection* conn, char** args, int start, int argc) {
    write_server (conn->parent);
    return OK;
}

response_t EXIT (Connection* conn, char** args, int start, int argc) {
    conn->parent->keep_alive = 0;
    return OK;
}

response_t BIN_SERVER (Connection* conn, char** args, int start, int argc) {
    size_t size = write_server (conn->parent);
    void* buffer = malloc (size);
    ((char*)buffer)[0] = 0;
    FILE* fp = fmemopen(buffer, size, "wb");
    size = write_server_fp (conn->parent, fp);
    rewind(fp);
    write (conn->fd, buffer, size);
    fclose (fp);
    free (buffer);
    
    response_t res = OK;
    res.len = 0;
    
    return res;
}

response_t HOSTWRITE (Connection* conn, char** args, int start, int argc) {
    if (conn->bounded_host == NULL)
        return FORBIDDEN;
    
    host_write_make_conf(conn->bounded_host);
    
    char* profile_dest;
    char* profile_src;
    struct stat __sym_buff;
    
    asprintf (&profile_src, "/usr/portage/profiles/%s/", conn->bounded_host->profile);
    asprintf (&profile_dest, "%s/%s/etc/portage/make.profile", conn->parent->location, conn->bounded_host->id);
    
    if (lstat (profile_dest, &__sym_buff) == 0) {
        unlink (profile_dest);
    }
    
    linfo ("Setting profile to %s", profile_src);
    if (symlink (profile_src, profile_dest) != 0) {
        lwarning("Failed to symlink %s!", profile_dest);
        return INTERNAL_ERROR;
    }
    
    return OK;
}
