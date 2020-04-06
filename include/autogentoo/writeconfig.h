#ifndef __AUTOGENTOO_WRITE_CONFIG_H__
#define __AUTOGENTOO_WRITE_CONFIG_H__

#include <stdio.h>
#include "server.h"
#include "host.h"

typedef struct __AutoGentoo_Memconfig AutoGentoo_Memconfig;

typedef enum {
	AUTOGENTOO_FILE_END = 0xffffffff,
	AUTOGENTOO_HOST = 0xfffffff0,
	
	// Because Host* is extensible this is required
	AUTOGENTOO_HOST_END = 0xaaaaaaaa,
	
	// Host entries for autogentoo extensions
	AUTOGENTOO_HOST_KERNEL = 0xbbbbbbbb,
	AUTOGENTOO_ACCESS_TOKEN = 0xdddddddd,
	AUTOGENTOO_SUDO_TOKEN = 0xcccccccc,
	AUTOGENTOO_SERVER_TOKEN = 0xfffff000
} AutoGentoo_WriteConfig;

struct __AutoGentoo_Memconfig {
	void* buffer_file;
	size_t size;
};

static AutoGentoo_Memconfig buffer_file;

/**
 * Write the server to file
 * @param server the server to write
 */
size_t write_server (Server* server);

size_t write_access_token_fp(AccessToken* token, FILE* fp);
AccessToken* read_access_token(FILE* fp);

/**
 * Write server to file given an open fp
 * @param server the server to write
 * @param fp the fp open to write to
 */
size_t write_server_fp (Server* server, FILE* fp);

/**
 * Write a host to file
 * @param host the host to write to file
 * @param fp the file to write to
 */
size_t write_host_fp (Host* host, FILE* fp);

/**
 * Write an active HostTemplate/stage to file
 * @param temp the template to write to file
 * @param fp the file to write to
 */
//size_t write_stage_fp (HostTemplate* temp, FILE* fp);

/**
 * Write a Template to file
 * @param temp the template to write to file
 * @param fp the file to write to
 */
//size_t write_template_fp (HostTemplate* temp, FILE* fp);

/**
 * Read server from localtion
 * @param location the localtion of the file
 * @return a new server that was read from the file
 */
Server* read_server (char* location, char* port, server_t opts);

/**
 * Read host from file
 * @param fp the open file to read from
 * @return the new host created after reading the file
 */
Host* read_host (FILE* fp);

/**
 * Read host binding from file
 * @param server The server to read into
 * @param dest the binding destination
 * @param fp the file to read from
 */
//void read_host_binding (Server* server, HostBind* dest, FILE* fp);

/**
 * Read a HostTemplate from file
 * @param server the parent server
 * @param dest the HostTemplate in which content will be written
 * @param fp the file to read from
 */
//void read_stage (Server* server, HostTemplate* dest, FILE* fp);

/**
 * Read a HostTemplate as a Template not Stage (dont read dest_dir) from file
 * @param server the parent server
 * @param dest the HostTemplate in which content will be written
 * @param fp the file to read from
 */
//void read_template(Server* server, HostTemplate* dest, FILE* fp);

/**
 * Writes a string to file (also writes the NULL terminator)
 * @param src the string to write
 * @param fp the file to write to
 */
size_t write_string (char* src, FILE* fp);

/**
 * Reads a string from file \
 * reads until the it reaches a NULL terminator
 * @param fp the file to read from
 * @return a dynamically allocated string that has been read from file
 */
char* read_string (FILE* fp);
int read_int_fd(int fd);
char* read_string_fd(int fd);
ssize_t write_int_fd(int fd, int i);

void worker_lock(int fd);
void worker_unlock(int fd);

/**
 * Write an integer to file
 * @param src the integer to write
 * @param fp the file to write to
 */
size_t write_int (int src, FILE* fp);

/**
 * Read an integer from file
 * @param fp the file to read from
 * @return the integer that has been read
 */
int read_int (FILE* fp);

void* read_void(size_t len, FILE* fp);
size_t write_void(void* ptr, size_t len, FILE* fp);

#endif