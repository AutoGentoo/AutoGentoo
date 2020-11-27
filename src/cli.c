//
// Created by atuser on 8/7/19.
//

#include <autogentoo/cli.h>
#include <pthread.h>

pthread_t cli_start(Server* server) {
    pthread_t pid;
    pthread_create(&pid, NULL, (void* (*)(void*)) cli, server);
    return pid;
}

void* cli(Server* server) {

}