#ifndef __AUTOGENTOO_HOST_H__
#define __AUTOGENTOO_HOST_H__

/**
 * @brief Holds information about a build environment
 * 
 * Most of this struct will contain information that is used in
 * /etc/portage/make.conf.
 */
typedef struct __Host Host;

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "response.h"
#include "server.h"
#include "kernel.h"
#include "host_environment.h"

#define AUTOGENTOO_HOST_ID_LENGTH 16

/**
 * @brief Are the chroot directories mounted
 * 
 * This will decide whether or not the system
 * is able to perform a chroot()
 * 
 * This is reset/updated after every restart (not persistant)
 */
typedef enum {
	CHR_NOT_MOUNTED,
	CHR_MOUNTED
} chroot_t;

typedef enum {
	HOST_ENV_VOID,   // Environment does not exist
	HOST_ENV_STAGE3, // Stage3 is unpacked but configs need writing
	HOST_ENV_BASE,   // Stage3 packages still need to be made into binaries
	HOST_ENV_READY   // Everything is ready to go
} host_env_t;

/**
 * @brief Holds information about a build environment
 * 
 * Most of this struct will contain information that is used in
 * /etc/portage/make.conf.
 */
struct __Host {
	Server* parent; //!< The parent server
	char* id; //!< The ID of the Host
	char* profile; //!< Portage profile, see possible values with eselect profile list
	char* hostname; //!< Hostname of the host (shows up in the graphical client)
	char* arch; //!<  The portage-arch (eg. amd64)
	
	char* abi;
	
	chroot_t chroot_status; //!< Is the chroot ready?
	host_env_t environment_status; //!< Is the environment ready to compile
	
	HostEnvironment* environment;
	Vector* kernel;
};

/**
 * Returns a randomly generated HOST_ID
 * @return a new host id
 */
char* host_id_new();

/**
 * Creates a new host given an ID
 * @param server the parent server
 * @param id the ID of the Host
 * @return a pointer to the Host
 */
Host* host_new(Server* server, char* id);

/**
 * Updates the current status of the host by reading its directory
 * @param host the host to update the status of
 */
void host_getstatus(Host* host);
int host_setstatus(Host* host);

/**
 * Writes the path of the host to dest
 * @param host the host to read path from
 * @param dest the destination of the Host's path
 */
char* host_path(Host* host, char* sub, ...);

/**
 * Free a host
 * @param host the host to free 
 */
void host_free(Host* host);

/* Request calls */

/**
 * Initialize extras
 * @param target target host
 */
void host_init_extras(Host* target);

int host_write_make_conf(Host* host);

#endif