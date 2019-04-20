#ifndef __AUTOGENTOO_SERVER_H__
#define __AUTOGENTOO_SERVER_H__

#include <autogentoo/hacksaw/hacksaw.h>
#include <sys/types.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <autogentoo/pool.h>

/**
 * @brief The main struct that hold hosts/bindings and other information
 *  - This is pretty much required throughout the entire program so keep it!
 */
typedef struct __Server Server;

/**
 * @brief RSA encrypted connections for secure access
 *  - All logins require encrypted connections
 */
typedef struct __EncryptServer EncryptServer;

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

/**
 * Server options for enabling/disabling server features
 */
typedef enum {
	ASYNC = 0x0, //!< Run the server in the terminal
	DAEMON = 0x1, //!< Run the server in background as a service
	NO_DEBUG = 0x0, //!< Turn off debug information
	DEBUG = 0x2, //!< Turn on debug information
	ENCRYPT = 0x4  //!< Start an encrypted socket
} server_t;

typedef enum {
	ENC_CERT_SIGN = 1 << 1,
	ENC_GEN_RSA = 1 << 2 | ENC_CERT_SIGN,
	ENC_GEN_CERT = 1 << 3 | ENC_CERT_SIGN,
	ENC_READ_CERT = 1 << 4,
	ENC_READ_RSA = 1 << 5,
} enc_server_t;

/**
 * Holds the status of a connection
 */
typedef enum {
	SERVER_ERROR, //!< recv() error
	CONNECTED, //!< Successfully connected
	FAILED, //!< Client disconnected closing the connection
	CLOSED //!< Closed successfully with connection_free()
} con_t;

typedef enum {
	COM_PLAIN,
	COM_RSA
} com_t;

#include "thread.h"
#include "queue.h"
#include "user.h"
#include <autogentoo/worker.h>

/**
 * @brief The main struct that hold hosts/bindings and other information
 * 
 * This is pretty much required throughout the entire program so keep it!
 */
struct __Server {
	char* location; //!< The working directory of the server
	char* port;  //!< The port to bind to
	server_t opts; //!< The options that the server was initilized with
	
	/* AutoGentoo Related */
	Vector* hosts; //!< A list of hosts
	
	volatile int keep_alive; //!< Set to 0 if you want the main loop to exit
	PoolHandler* pool_handler; //!< For socket requests
	WorkerHandler* job_handler; //!< For running jobs
	
	pid_t pid;
	pthread_t pthread;
	int socket;
	
	/* Secure server */
	EncryptServer* rsa_child;
	
	/* Authentication */
	char* autogentoo_org_token; //!< Just a dup of the key
	
	Map* auth_tokens;
};

#ifndef connection_read
#define connection_read(dest, size) \
conn->communication_type == COM_RSA ? \
	SSL_read(conn->encrypted_connection, dest, (int)size) : read(conn->fd, dest, size)
#endif

#ifndef connection_write
#define connection_write(conn, src, size) \
conn->communication_type == COM_RSA ? SSL_write(conn->encrypted_connection, src, (int)size) : write (conn->fd, src, size);
#endif

struct __EncryptServer {
	Server* parent;
	char* port;
	pthread_t pid;
	enc_server_t opts;
	
	char* rsa_path;
	char* cert_path;
	
	int socket;
	
	/** ONLY AUTOGENTOO_S
	 * 2048 RSA encryption
	 * Serverside encryption
	 **/
	X509* certificate;
	SSL_CTX* context;
	RSA* key_pair;
};

/**
 * @brief Bind an IP to a host
 * 
 * This is used when a request is made. 
 * The IP of the request is matched to a Host using this struct
 */
struct __HostBind {
	char* ip; //!< The IP to bind the host to
	struct __Host* host; //!< The target of the host binding
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
	struct __Server* parent; //!< The parent server of the connection
	void* request; //!< The entire request
	size_t size;
	char* ip; //!< The IP of the connected client
	int fd; //!< The file descriptor that points to the open connections
	int worker;
	con_t status; //!< The status of the current Connection
	com_t communication_type; //!< Are we using an encryption or not
	
	/** ONLY AUTOGENTOO_S
	 * 2048 RSA encryption
	 * Clientside encryption
	 **/
	SSL* encrypted_connection;
	int encrypted_fd;
};

#include "host.h"

/**
 * Creates a new server ready to start
 * @param location The working directory of the server (Hosts are installed here)
 * @param port the port to start on (default: 9490, debug: 9491)
 * @param opts here you are able to enable the DAEMON or DEBUG features
 * @return a pointer to the newly created server
 */
Server* server_new(char* location, char* port, server_t opts);

void server_encrypt_start(EncryptServer* server);
EncryptServer* server_encrypt_new(Server* parent, char* port, char* cert_path, char* rsa_path, enc_server_t opts);

/**
 * Creates a new Connection given a file desciptor from accept()
 * @param server the parent server
 * @param conn_fd the fd taken from accept()
 * @return a pointer to the newly created Connection
 */
Connection* connection_new(Server* server, int conn_fd);

Connection* connection_new_tls(EncryptServer* server, int accepted_fd);

/**
 * Start the server and allow connections
 * @param server the server to start
 */
void server_start(Server* server);

/**
 * Search for a host given an ID
 * @param server the server to look in
 * @param id the ID to search for
 * @return a pointer to the Host that was found, NULL if not found
 */
Host* server_get_host(Server* server, char* id);

/**
 * Meta-function called by server_start (DO NOT CALL THIS)
 * @param port the port to bind on
 * @return the fd to read from
 */
int server_init(short port);

/**
 * Handle the conn's request
 * @param conn the connection to read request from
 */
void server_respond(Connection* conn, int worker_index);

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

void server_encrypt_free (EncryptServer* server);

void server_kill(Server* server);

void kill_encrypt_server(int sig);

void handle_sigint (int sig);

char* server_get_path (Server* parent, char* path);

extern Server* srv;

#endif