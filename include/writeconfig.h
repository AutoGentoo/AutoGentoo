#ifndef __AUTOGENTOO_WRITE_CONFIG_H__
#define __AUTOGENTOO_WRITE_CONFIG_H__

#include <stdio.h>
#include "server.h"
#include "host.h"
#include "stage.h"

/**
 * Write the server to file
 * @param server the server to write
 */
void write_server(Server* server);

/**
 * Write server to file given an open fp
 * @param server the server to write
 * @param fp the fp open to write to
 */
void write_server_fp(Server* server, FILE* fp);

/**
 * Write a host to file
 * @param host the host to write to file
 * @param fp the file to write to
 */
void write_host_fp(Host* host, FILE* fp);

/**
 * Write a binding to file
 * @param bind the binding to write to file
 * @param fp the file to write to
 */
void write_host_binding_fp(HostBind* bind, FILE* fp);

/**
 * Write an active HostTemplate/stage to file
 * @param temp the template to write to file
 * @param fp the file to write to
 */
void write_stage_fp(HostTemplate* temp, FILE* fp);

/**
 * Read server from localtion
 * @param location the localtion of the file
 * @return a new server that was read from the file
 */
Server* read_server(char* location);

/**
 * Read host from file
 * @param fp the open file to read from
 * @return the new host created after reading the file
 */
Host* read_host(FILE* fp);

/**
 * Read host binding from file
 * @param server The server to read into
 * @param dest the binding destination
 * @param fp the file to read from
 */
void read_host_binding(Server* server, HostBind* dest, FILE* fp);

/**
 * Read a HostTemplate from file
 * @param server the parent server
 * @param dest the HostTemplate in which content will be written
 * @param fp the file to read from
 */
void read_stage(Server* server, HostTemplate* dest, FILE* fp);

/**
 * Writes a string to file (also writes the NULL terminator)
 * @param src the string to write
 * @param fp the file to write to
 */
void write_string(char* src, FILE* fp);

/**
 * Reads a string from file \
 * reads until the it reaches a NULL terminator
 * @param fp the file to read from
 * @return a dynamically allocated string that has been read from file
 */
char* read_string(FILE* fp);

/**
 * Write an integer to file
 * @param src the integer to write
 * @param fp the file to write to
 */
void write_int(int src, FILE* fp);

/**
 * Read an integer from file
 * @param fp the file to read from
 * @return the integer that has been read
 */
int read_int(FILE* fp);

#endif