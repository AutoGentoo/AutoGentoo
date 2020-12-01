//
// Created by tumbar on 11/30/20.
//

#ifndef AUTOGENTOO_MESSAGE_H
#define AUTOGENTOO_MESSAGE_H

#include "hacksaw/global.h"
#include <Python.h>

typedef struct Message_prv Message;
typedef struct MessageDataFrame_prv MessageDataFrame;

/* Use this messaging tactic for general purpose IPC
 * with low bandwith parameter passing
 *
 * Up to 6 * sizeof(void*) of data
 * */
struct Message_prv {
    U32 token;

    /* General purpose parameters */
    struct {
        PXX val1;
        PXX val2;
        PXX val3;
        PXX val4;
        PXX val5;
        PXX val6;
    } data;
};

/* If high band of data needs to be sent/received,
 * use a data frame */
struct MessageDataFrame_prv {
    Message parent;

    U32 size;
    void* data;
};

#endif //AUTOGENTOO_MESSAGE_H
