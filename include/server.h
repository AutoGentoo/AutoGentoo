#ifndef __AUTOGENTOO_SERVER_H__
#define __AUTOGENTOO_SERVER_H__

typedef struct __Server Server;
typedef struct __Connection Connection;

#include <hacksaw/tools.h>
#include "host.h"
#include <sys/types.h>

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
    SmallMap* host_bindings;
    Vector* hosts;
    Vector* connections;
    char port[5];
    server_t opts;
};

struct __Connection {
    Server* parent;
    Host* bounded_host; // NULL if unbounded
    char* request;
    char* ip;
    int fd;
    pid_t pid;
    con_t status;
};

Server* server_new (char* location, char* port, server_t opts);
Connection* connection_new (Server* server, int conn_fd);
void server_start (Server* server);
void server_bind (Connection* conn, Host** host);
int server_init (char* port);
void server_respond (Connection* conn);
void connection_free (Connection* conn);
void kill_finished (int sig);
void daemonize(char* _cwd);

#endif