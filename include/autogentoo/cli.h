//
// Created by atuser on 8/7/19.
//

#include "server.h"

#ifndef AUTOGENTOO_CLI_H
#define AUTOGENTOO_CLI_H

pthread_t cli_start(Server* server);

void* cli(Server* server);

#endif //AUTOGENTOO_CLI_H
