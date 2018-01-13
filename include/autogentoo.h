#ifndef __AUTOGENTOO_H__
#define __AUTOGENTOO_H__

#include "server.h"
#include "writeconfig.h"
#include "endian_convert.h"

/* Port to 9491 so we don't overlap with our main service */
#define AUTOGENTOO_DEBUG

#define AUTOGENTOO_NO_THREADS

#ifdef AUTOGENTOO_DEBUG
#define AUTOGENTOO_PORT 9491
#else
#define AUTOGENTOO_PORT 9490
#endif

int main (int argc, char** argv);

#endif