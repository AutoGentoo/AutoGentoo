//
// Created by tumbar on 11/30/20.
//

#ifndef AUTOGENTOO_TCP_SERVER_H
#define AUTOGENTOO_TCP_SERVER_H

#include <pthread.h>
#include <hacksaw/object.h>
#include <Python.h>
#include <hacksaw/queue.h>

#ifndef TCP_WORKER_COUNT
#define TCP_WORKER_COUNT 8
#endif

typedef struct TCPServer_prv TCPServer;
typedef struct Request_prv Request;

typedef enum
{
    NETWORK_TYPE_NET,
    NETWORK_TYPE_UNIX
} net_type_t;

typedef union
{
    struct {
        U16 port;
        U32 ip; /* Support only IPv4 */
    } net_addr;
    char* path; /* Unix domain endpoint */
} Address;

struct TCPServer_prv
{
    PyObject_HEAD
    int socket;
    net_type_t type;
    Address address;

    /* Multi-threaded related */
    pthread_t run_thread;

    U8 keep_alive;
    U8 is_alive;
    U32 worker_n;
    pthread_t* worker_threads;

    Queue* request_queue;
    pthread_mutex_t lock;
    pthread_cond_t cond;

    PyObject* callback;
};

struct Request_prv
{
    REFERENCE_OBJECT
    int client;
};

extern PyTypeObject TCPServerType;

#endif //AUTOGENTOO_TCP_SERVER_H
