#ifndef __AUTOGENTOO_SERVER_H__
#define __AUTOGENTOO_SERVER_H__

typedef struct __Server Server;
typedef struct __HostBind HostBind;
typedef struct __Connection Connection;

#include <hacksaw/tools.h>
#include "host.h"
#include <sys/types.h>
#include <pthread.h>

typedef enum {
    ASYNC = 0x0,
    DAEMON = 0x1,
    NO_DEBUG = 0x0,
    DEBUG = 0x2
} server_t;

typedef enum {
    SERVER_ERROR, // recv() error
    CONNECTED,
    FAILED, // Client disconnected closing the connection
    CLOSED // Closed successfully with connection_free()
} con_t;

struct __Server {
    char* location;
    int port;
    server_t opts;
    Vector* hosts;
    Vector* host_bindings;
};

struct __HostBind {
    char* ip;
    Host* host;
};

struct __Connection {
    Server* parent;
    Host* bounded_host; // NULL if unbounded
    char* request;
    char* ip;
    int fd;
    pthread_t pid;
    con_t status;
};

Server* server_new (char* location, int port, server_t opts);
Connection* connection_new (Server* server, int conn_fd);
Host* server_get_active (Server* server, char* ip);
void server_start (Server* server);
void server_bind (Connection* conn, Host* host);
Host* server_host_search (Server* server, host_id id);
int server_init (char* port);
void server_respond (Connection* conn);
void connection_free (Connection* conn);
void daemonize(char* _cwd);

#endif