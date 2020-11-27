//
// Created by atuser on 2/24/20.
//

#ifndef AUTOGENTOO_NET_H
#define AUTOGENTOO_NET_H

typedef struct __NetworkBridge NetworkBridge;

#include "namespace.h"

static int interface_n = 0;

struct __NetworkInterface {

};

struct __NetworkBridge {
    pid_t target;

    int interface_n; /* Interface number on parent pid */

    char* init_command;
    char* addr_command;
    char* addr_command_child;

    char* name_parent; /* veth[interface_n] */
    char* name_child; /* eth0 */

    char* address_parent; /* 10.0.0.[2 * interface_n] */
    char* address_child; /* 10.0.0.[2 * interface_n + 1] */
};


NetworkBridge* network_bridge_new(pid_t pid);

/**
 * Run the parent initialization for the network interface
 * Set the ip of this interface to the set addr
 * @param ni interface to perform a parent init on
 * @return error code (0 for success)
 */
int network_bridge_pinit(NetworkBridge* ni);

/**
 * Run the child initialization
 * Set ip to the child addr specified
 * @param ni interface to perform a child init on
 * @return error code (0 for success)
 */
int network_bridge_cinit(NetworkBridge* ni);


#endif //AUTOGENTOO_NET_H
