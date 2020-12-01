//
// Created by tumbar on 12/1/20.
//

#ifndef AUTOGENTOO_TCP_CLIENT_H
#define AUTOGENTOO_TCP_CLIENT_H

#include <Python.h>
#include "tcp_server.h"
#include "message.h"

void tcp_client_send_message(Address address,
                             net_type_t addr_type,
                             MessageFrame* messsage,
                             MessageFrame* reply);


#endif //AUTOGENTOO_TCP_CLIENT_H
