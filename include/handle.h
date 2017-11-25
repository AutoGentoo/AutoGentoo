#ifndef __AUTOGENTOO_HANDLE_H__
#define __AUTOGENTOO_HANDLE_H__

#include "server.h"
#include "response.h"

typedef response_t (*SHFP)(Connection*, char**, int);
typedef struct __RequestLink RequestLink;

struct __RequestLink {
    char* request_ident;
    SHFP call;
};

extern RequestLink requests[];

SHFP parse_request (char* parse_line, char** args);
response_t GET (Connection*, char** args, int start);
response_t INSTALL (Connection* conn, char** args, int start);

/* SRV Configure requests */
response_t SRV_CREATE (Connection* conn, char** args, int start);
//response_t SRV_DEVCREATE (Connection* conn, char** args, int start);
response_t SRV_EDIT (Connection* conn, char** args, int start);
response_t SRV_ACTIVATE (Connection* conn, char** args, int start);
response_t SRV_HOSTREMOVE (Connection* conn, char** args, int start);

/* SRV Utility request */
response_t SRV_INIT (Connection* conn, char** args, int start);
response_t SRV_STAGE1 (Connection* conn, char** args, int start);
response_t SRV_MNTCHROOT (Connection* conn, char** args, int start);

/* SRV Metadata requests */
response_t SRV_GETHOST (Connection* conn, char** args, int start);
response_t SRV_GETHOSTS (Connection* conn, char** args, int start);
response_t SRV_GETACTIVE (Connection* conn, char** args, int start);
response_t SRV_GETSPEC (Connection* conn, char** args, int start);

/* SRV Kernel request (unimplmented) */

#endif
