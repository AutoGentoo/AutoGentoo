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

/**
 * Creates a new server ready to start
 * @param location The working directory of the server (Hosts are installed here)
 * @param port the port to start on (default: 9490, debug: 9491)
 * @param opts here you are able to enable the DAEMON or DEBUG features
 * @return a pointer to the newly created server
 */
Server* server_new (char* location, int port, server_t opts);

/**
 * Creates a new Connection given a file desciptor from accept()
 * @param server the parent server
 * @param conn_fd the fd taken from accept()
 * @return a pointer to the newly created Connection
 */
Connection* connection_new (Server* server, int conn_fd);

/**
 * Get the host that is bounded to the given IP address
 * @param server the parent server to look in
 * @param ip the ip to search for
 * @return a pointer to the bounded host, NULL if was not found
 */
Host* server_get_active (Server* server, char* ip);

/**
 * Start the server and allow connections
 * @param server the server to start
 */
void server_start (Server* server);

/**
 * Bind an IP address to a host
 * @param conn the connection that holds our IP
 * @param host the host to bind to
 */
void server_bind (Connection* conn, Host* host);

/**
 * Search for a host given an ID
 * @param server the server to look in
 * @param id the ID to search for
 * @return a pointer to the Host that was found, NULL if not found
 */
Host* server_host_search (Server* server, host_id id);

/**
 * Meta-function called by server_start (DO NOT CALL THIS)
 * @param port the port to bind on
 * @return the fd to read from
 */
int server_init (char* port);

/**
 * Handle the conn's request
 * @param conn the connection to read request from
 */
void server_respond (Connection* conn);

/**
 * Free the completed connection
 * @param conn the connection to free
 */
void connection_free (Connection* conn);

/**
 * Fork this process to a daemon and disconnect from the terminal
 * @param _cwd The working directory to cd to
 */
void daemonize(char* _cwd);

#endif