#ifndef __AUTOGENTOO_WRITE_CONFIG_H__
#define __AUTOGENTOO_WRITE_CONFIG_H__

#include <stdio.h>
#include "server.h"
#include "host.h"

void write_server (Server* server);
void write_server_fp (Server* server, FILE* fp);
void write_host_fp (Host* bind, FILE* fp);
void write_host_binding_fp (HostBind* host, FILE* fp);

Server* read_server (char* location);
Host* read_host (FILE* fp);
void read_host_binding (Server* server, HostBind* dest, FILE* fp);

void write_string (char* src, FILE* fp);
char* read_string (FILE* fp);
void write_int (int src, FILE* fp);
int read_int (FILE* fp);

#endif