//
// Created by atuser on 1/12/18.
//

#ifndef AUTOGENTOO_GCLIENT_INTERFACE_H
#define AUTOGENTOO_GCLIENT_INTERFACE_H

#include <hacksaw/tools.h>
#include <autogentoo/autogentoo.h>

typedef Host host_t;
typedef Server server_t;


server_t* autogentoo_connect (char* ip, int port);


#endif //AUTOGENTOO_GCLIENT_INTERFACE_H
