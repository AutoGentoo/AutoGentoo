//
// Created by atuser on 3/24/19.
//

#ifndef AUTOGENTOO_SSL_STREAM_H
#define AUTOGENTOO_SSL_STREAM_H

#include <stdio.h>
#include <autogentoo/api/ssl_wrap.h>

typedef struct __SMWConnection SMWConnection;
typedef struct __SMWServer SMWServer;
typedef struct __SMWPool SMWPool;

#define SMW_STREAM_BUFFER_SIZE 4

/* RUNNING in the API context not SERVER context */

/**
 * The webclient will send a request with the websocket
 * information. client is an ssocket connection. Send
 * data in buffer_size chunks
 */
struct __SMWConnection {
	SMWServer* parent;
	
	SSL* write_side; /* autogentoo.org side */
	int write_side_sock;
	
	SSocket* read_side; /* AutoGentoo side */
	
	SMWPool* thread;
};

/* Client ---> Server */
/* https://autogentoo.org ---> SMWServer ---> AutoGentoo */
/*                            | Django |                 */

struct __SMWPool {
	SMWPool* next;
	SMWPool* prev;
	SMWConnection* child;
	pthread_t pid;
};

/**
 * Start this server alongside with django to act as the
 * middle-man between C and javascript (Server and Client)
 */
struct __SMWServer {
	int socket;
	char* port;
	pthread_t pid;
	
	char* (*django_callback)(char* server_id);
	
	/* Open pools as needed */
	SMWPool* pool_head;
	pthread_mutex_t pool_mutex;
	
	char* cert_file;
	char* rsa_file;
	
	X509* certificate;
	SSL_CTX* context;
	RSA* key_pair;
	
	int keep_alive;
};

SMWServer* smw_server_new (char* port, char* cert_file, char* rsa_file);
pthread_t smw_server_start(SMWServer* server);
void smw_server_free(SMWServer* server);
SMWConnection* smw_server_connect(SMWServer* server, int fd);

SMWPool* smw_fork(SMWServer* server, int accepted_sock);
void smw_stream(SMWConnection* conn);

#endif //AUTOGENTOO_SSL_STREAM_H
