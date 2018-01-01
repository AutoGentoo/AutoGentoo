#include <stdio.h>
#include <stdlib.h>
#include <server.h>
#include <string.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <handle.h>
#include <errno.h>
#include <stage.h>
#include "writeconfig.h"

Server* server_new(char* location, int port, server_t opts) {
    Server* out = malloc(sizeof(Server));
    out->hosts = vector_new(sizeof(Host*), REMOVE | UNORDERED);
    out->stages = small_map_new(sizeof(HostTemplate*), 5);
    out->host_bindings = vector_new(sizeof(HostBind), REMOVE | UNORDERED);
    out->location = strdup(location);
    chdir(out->location);
    out->opts = opts;
    out->port = port;
    
    return out;
}

Host* server_get_active(Server* server, char* ip) {
    int i;
    for (i = 0; i != server->host_bindings->n; i++) {
        HostBind* current_bind = (HostBind*)vector_get(server->host_bindings, i);
        if (strcmp(ip, current_bind->ip) == 0) {
            return current_bind->host;
        }
    }
    
    return NULL;
}

HostBind* prv_server_get_active_location(Server* server, char* ip) {
    int i;
    for (i = 0; i != server->host_bindings->n; i++) {
        HostBind* current_bind = (HostBind*)vector_get(server->host_bindings, i);
        if (strcmp(ip, current_bind->ip) == 0) {
            return current_bind;
        }
    }
    
    return NULL;
}

Connection* connection_new(Server* server, int conn_fd) {
    Connection* out = malloc(sizeof(Connection));
    out->parent = server;
    out->fd = conn_fd;
    
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int res = getpeername(out->fd, (struct sockaddr*)&addr, &addr_size);
    out->ip = strdup(inet_ntoa(addr.sin_addr));
    out->bounded_host = server_get_active(out->parent, out->ip);
    
    return out;
}

void server_start(Server* server) {
    struct sockaddr_in clientaddr;
    socklen_t addrlen;
    
    char port[5];
    sprintf (port, "%d", server->port);
    
    int listenfd = server_init(port);
    
    linfo("Server started on port %d", server->port);
    
    if (server->opts & DAEMON) {
        daemonize(server->location);
    }
    
    addrlen = sizeof(clientaddr);
    
    while (1) { // Main accept loop
        int temp_fd = accept(listenfd, (struct sockaddr*)&clientaddr, &addrlen);
        if (temp_fd < 3) {
            lwarning("accept() error");
            continue;
        }
        if (fcntl(temp_fd, F_GETFD) == -1 || errno == EBADF) {
            lwarning("Bad fd on accept()");
            fflush(stdout);
            continue;
        }
        
        pthread_t p_pid;
        
        Connection* current_conn = connection_new(server, temp_fd);
        if (pthread_create(&p_pid, NULL, (void* (*)(void*))server_respond, current_conn)) {
            lerror("Error creating thread");
            fflush(stdout);
            exit(1);
        }
    }
}

int server_init(char* port) {
    struct addrinfo hints, * res, * p;
    int listenfd = -1;
    
    // getaddrinfo for host
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, port, &hints, &res) != 0) {
        lerror("getaddrinfo() error");
        exit(1);
    }
    // socket and bind
    for (p = res; p != NULL; p = p->ai_next) {
        listenfd = socket(p->ai_family, p->ai_socktype, 0);
        if (listenfd == -1)
            continue;
        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
            lerror("setsockopt(SO_REUSEADDR) failed");
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break;
    }
    if (p == NULL) {
        lerror("socket() or bind()");
        switch (errno) {
            case EADDRINUSE:
                lerror("Port %s in use", port);
                break;
            case EACCES:
                lerror("Permission denied");
                break;
            case ENETUNREACH:
                lerror("Network unreachable");
                break;
            default:
                break;
        }
        exit(1);
    }
    
    freeaddrinfo(res);
    
    // listen for incoming connections
    
    if (listen(listenfd, 64) != 0) {
        lerror("listen() error");
        exit(1);
    }
    
    return listenfd;
}

void connection_free(Connection* conn) {
    if (conn->status != CLOSED) {
        shutdown(conn->fd, SHUT_RDWR);
        if (fcntl(conn->fd, F_GETFD) != -1 || errno != EBADF) {
            close(conn->fd);
        }
        conn->status = CLOSED;
    }
    
    free(conn->request);
    free(conn->ip);
    free(conn);
}

void server_respond(Connection* conn) {
    conn->pid = pthread_self();
    
    /* Read the request */
    conn->request = malloc(2048);
    ssize_t total_read = 0, current_bytes = 0;
    size_t buffer_size = 2048;
    total_read += current_bytes = read(conn->fd, conn->request, 2048);
    while (current_bytes == 2048) {
        buffer_size += 2048;
        conn->request = realloc(conn->request, buffer_size);
        current_bytes = read(conn->fd, conn->request + total_read, 2048);
        total_read += current_bytes;
    }
    
    if (total_read < 0) { // receive error
        lerror("recv() error");
        conn->status = SERVER_ERROR;
        return;
    } else if (total_read == 0) { // receive socket closed
        lwarning("Client disconnected upexpectedly.");
        conn->status = FAILED;
        return;
    } else {
        conn->status = CONNECTED;
    }
    
    char* args[4]; // Written to by parse_request
    
    char* request_line;
    int split_i = (int)(strchr(conn->request, '\n') - conn->request);
    request_line = malloc((size_t)split_i + 1);
    strncpy (request_line, conn->request, split_i);
    request_line[split_i] = 0;
    
    SHFP call = parse_request(request_line, args);
    
    linfo("handle %s on pthread_t 0x%llx (%s)", conn->ip, conn->pid, request_line);
    
    response_t res;
    
    if (call == NULL) {
        res = BAD_REQUEST;
    } else {
        res = (*call)(conn, args, split_i + 1);
    }
    
    free(request_line);
    
    if (res.len != 0) {
        rsend(conn, res);
    }
    linfo("request 0x%llx: %s (%d)", conn->pid, res.message, res.code);
    write_server(conn->parent);
    
    connection_free(conn);
    printf("freed\n");
    fflush(stdout);
}

void server_bind(Connection* conn, Host* host) {
    HostBind* loc = prv_server_get_active_location(conn->parent, conn->ip);
    if (loc == NULL) {
        HostBind new_binding = {.ip = strdup(conn->ip), .host = host};
        vector_add(conn->parent->host_bindings, &new_binding);
    } else {
        loc->host = host;
    }
}

Host* server_host_search(Server* server, host_id id) {
    int i;
    for (i = 0; i != server->hosts->n; i++) {
        Host* current_host = *(Host**)vector_get(server->hosts, i);
        if (strcmp((char*)id, current_host->id) == 0) {
            return current_host;
        }
    }
    
    return NULL;
}

void daemonize(char* _cwd) {
    pid_t pid, sid;
    int fd;
    
    /* already a daemon */
    if (getppid() == 1)
        return;
    
    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(1);
    }
    
    if (pid > 0) {
        linfo("Forked to pid: %d", (int)pid);
        linfo("Moving to background");
        fflush(stdout);
        exit(0); /*Killing the Parent Process*/
    }
    
    /* At this point we are executing as the child process */
    
    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        exit(1);
    }
    
    /* Change the current working directory. */
    if ((chdir(_cwd)) < 0) {
        exit(1);
    }
    
    
    fd = open("/dev/null", O_RDWR, 0);
    
    if (fd != -1) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        
        if (fd > 2) {
            close(fd);
        }
    }
    
    /*resettign File Creation Mask */
    umask(027);
}