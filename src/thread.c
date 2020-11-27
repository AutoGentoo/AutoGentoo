//
// Created by atuser on 1/14/18.
//

#include <autogentoo/thread.h>
#include <memory.h>

Connection* thread_get_conn(ThreadHandler* handler, pthread_t thread) {
    int i;
    /*
    for (i = 0; i != handler->conn_max; i++)
        if (handler->threads[i]->pid == thread)
            return handler->threads[i];
    */
    return NULL;
}

ThreadHandler* thread_handler_new(size_t conn_max) {
    ThreadHandler* out = malloc(sizeof(ThreadHandler));
    out->threads = calloc(conn_max, sizeof(pthread_t));
    out->conn_max = conn_max;

    return out;
}

void thread_join(ThreadHandler* handler, pthread_t thread) {
    pthread_join(thread, NULL);
    int i;
    /*
    for (i = 0; i != handler->conn_max; i++)
        if (handler->threads[i] && handler->threads[i]->pid == thread)
            handler->threads[i] = NULL;
    */
}

void thread_register(ThreadHandler* handler, pthread_t thread, Connection* conn) {
    int i;
    for (i = 0; i != handler->conn_max; i++) {
        Connection** k = &handler->threads[i];
        if (!(*k)) {
            *k = conn;
            return;
        }
    }

    lerror("No more threads availiable.");
}

void thread_handler_join_all(ThreadHandler* handler) {
    int i;
    for (i = 0; i != handler->conn_max; i++) {
        Connection** k = &handler->threads[i];
        if (*k) {
            //pthread_join((*k)->pid, NULL);
            *k = NULL;
        }
    }
}

void thread_handler_free(ThreadHandler* handler) {
    free(handler->threads);
    free(handler);
}