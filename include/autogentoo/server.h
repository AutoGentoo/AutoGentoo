#ifndef __AUTOGENTOO_SERVER_H__
#define __AUTOGENTOO_SERVER_H__

/**
 * @brief The main struct that hold hosts/bindings and other information
 *  - This is pretty much required throughout the entire program so keep it!
 */
typedef struct __Server Server;

/**
 * @brief Bind an IP to a host
 *  - This is used when a request is made. 
 *  - The IP of the request is matched to a Host using this struct
 */
typedef struct __HostBind HostBind;

/**
 * @brief Holds information about a connection
 *  - A new connection will initilized after the return of accept().
 *  - The request is immediately after the connection is made
 *  - The bounded_host will be set as well (NULL if the IP is not bounded)
 *  - Since every connection runs in its own thread the pthread_t is kept here as well
 */
typedef struct __Connection Connection;

#include <autogentoo/hacksaw/hacksaw.h>
#include "host.h"
#include "thread.h"
#include <sys/types.h>
#include <pthread.h>

/**
 * Server options for enabling/disabling server features
 */
typedef enum {
	ASYNC = 0x0, //!< Run the server in the terminal
	DAEMON = 0x1, //!< Run the server in background as a service
	NO_DEBUG = 0x0, //!< Turn off debug information
	DEBUG = 0x2 //!< Turn on debug information
} server_t;

/**
 * Holds the status of a connection
 */
typedef enum {
	SERVER_ERROR, //!< recv() error
	CONNECTED, //!< Successfully connected
	FAILED, //!< Client disconnected closing the connection
	CLOSED //!< Closed successfully with connection_free()
} con_t;

/**
 * @brief The main struct that hold hosts/bindings and other information
 * 
 * This is pretty much required throughout the entire program so keep it!
 */
struct __Server {
	char* location; //!< The working directory of the server
	char* port;  //!< The port to bind to
	server_t opts; //!< The options that the server was initilized with
	Vector* hosts; //!< A list of hosts
	SmallMap* stages; //!< A list of active templates awaiting handoff to a host
	Vector* host_bindings; //!< A list of host bindings
	Vector* templates; //!< A list of availiable host templates
	
	volatile int keep_alive; //!< Set to 0 if you want the main loop to exit
	ThreadHandler* thandler;
	pid_t pid;
};

/**
 * @brief Bind an IP to a host
 * 
 * This is used when a request is made. 
 * The IP of the request is matched to a Host using this struct
 */
struct __HostBind {
	char* ip; //!< The IP to bind the host to
	Host* host; //!< The target of the host binding
};

/**
 * @brief Holds information about a connection
 * 
 * A new connection will initilized after the return of accept().
 * The request is immediately after the connection is made
 * The bounded_host will be set as well (NULL if the IP is not bounded)
 * Since every connection runs in its own thread the pthread_t is kept here as well
 */
struct __Connection {
	Server* parent; //!< The parent server of the connection
	Host* bounded_host; //!< The Host bounded to the connections IP address (NULL if unbounded)
	void* request; //!< The entire request
	char* ip; //!< The IP of the connected client
	int fd; //!< The file descriptor that points to the open connections
	pthread_t pid; //!< The pid of the pthread that the handle is runnning in
	con_t status; //!< The status of the current Connection
};

/**
 * Creates a new server ready to start
 * @param location The working directory of the server (Hosts are installed here)
 * @param port the port to start on (default: 9490, debug: 9491)
 * @param opts here you are able to enable the DAEMON or DEBUG features
 * @return a pointer to the newly created server
 */
Server* server_new(char* location, char* port, server_t opts);

/**
 * Creates a new Connection given a file desciptor from accept()
 * @param server the parent server
 * @param conn_fd the fd taken from accept()
 * @return a pointer to the newly created Connection
 */
Connection* connection_new(Server* server, int conn_fd);

/**
 * Get the host that is bounded to the given IP address
 * @param server the parent server to look in
 * @param ip the ip to search for
 * @return a pointer to the bounded host, NULL if was not found
 */
Host* server_get_active(Server* server, char* ip);

/**
 * Start the server and allow connections
 * @param server the server to start
 */
void server_start(Server* server);

/**
 * Bind an IP address to a host
 * @param conn the connection that holds our IP
 * @param host the host to bind to
 */
void server_bind(Connection* conn, Host* host);

/**
 * Search for a host given an ID
 * @param server the server to look in
 * @param id the ID to search for
 * @return a pointer to the Host that was found, NULL if not found
 */
Host* server_host_search(Server* server, host_id id);

/**
 * Meta-function called by server_start (DO NOT CALL THIS)
 * @param port the port to bind on
 * @return the fd to read from
 */
int server_init(char* port);

/**
 * Handle the conn's request
 * @param conn the connection to read request from
 */
void server_respond(Connection* conn);

/**
 * Free the completed connection
 * @param conn the connection to free
 */
void connection_free(Connection* conn);

/**
 * Fork this process to a daemon and disconnect from the terminal
 * @param _cwd The working directory to cd to
 */
void daemonize(char* _cwd);

/**
 * Free a server
 * @param server server to free 
 */
void server_free(Server* server);

void server_kill(Server* server);

#endif