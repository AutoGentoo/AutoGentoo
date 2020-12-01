//
// Created by tumbar on 11/30/20.
//

#ifndef AUTOGENTOO_MESSAGE_H
#define AUTOGENTOO_MESSAGE_H

#include "hacksaw/global.h"
#include <Python.h>

typedef struct Message_prv Message;
typedef struct MessageFrame_prv MessageFrame;

/* Use this messaging tactic for general purpose IPC
 * with low bandwith parameter passing
 *
 * Up to 6 * sizeof(void*) of data
 * */
struct Message_prv
{
    U32 token;

    /* General purpose parameters */
    struct
    {
        U64 val1;
        U64 val2;
        U64 val3;
        U64 val4;
        U64 val5;
        U64 val6;
    } data;
};

/* If high band of data needs to be sent/received,
 * use a data frame */
struct MessageFrame_prv
{
    Message parent;

    U64 size;
    void* data;
};

PyObject* PyMessage_FromMessageFrame(MessageFrame*);
int PyMessage_AsMessageFrame(PyObject*, MessageFrame*);

#endif //AUTOGENTOO_MESSAGE_H
