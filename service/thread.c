//
// Created by atuser on 1/14/18.
//

#include <thread.h>
#include <stdlib.h>
#include <memory.h>

ThreadHandler* thread_handler_new (size_t conn_max) {
    ThreadHandler* out = malloc (sizeof (ThreadHandler));
    out->threads = calloc (conn_max, sizeof (pthread_t));
    out->conn_max = conn_max;
    
    return out;
}

void thread_join (ThreadHandler* handler, pthread_t thread) {
    int i;
    for (i = 0; i != handler->conn_max; i++) {
        pthread_t k = handler->threads[i];
        if (k == thread)
            memset (&handler->threads[i], 0, sizeof (pthread_t));
    }
    
    pthread_join(thread, NULL);
}

void thread_register (ThreadHandler* handler, pthread_t thread) {
    int i;
    for (i = 0; i != handler->conn_max; i++) {
        pthread_t k = handler->threads[i];
        if (k == 0)
            handler->threads[i] = thread;
    }
}

void thread_handler_join_all (ThreadHandler* handler) {
    int i;
    for (i = 0; i != handler->conn_max; i++) {
        pthread_t k = handler->threads[i];
        if (k != 0) {
            memset (&handler->threads[i], 0, sizeof (pthread_t));
            pthread_join(k, NULL);
        }
    }
}

void thread_handler_free (ThreadHandler* handler) {
    free (handler->threads);
    free (handler);
}