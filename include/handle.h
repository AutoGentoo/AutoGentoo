#ifndef __AUTOGENTOO_HANDLE_H__
#define __AUTOGENTOO_HANDLE_H__

#include "server.h"
#include "response.h"

/**
 * A function pointer for to handle requests
 * @param conn the connection of the request
 * @param args a list of arguments passed to the handler
 * @param the index to start reading the request at
 */
typedef response_t (* SHFP) (Connection* conn, char** args, int start, int argc);

/**
 * Links a string to a request handler
 */
typedef struct __RequestLink RequestLink;

/**
 * Links a string to a request handler
 */
struct __RequestLink {
    char* request_ident; //!< The string that matches the request
    SHFP call; //!< A pointer to the function handler
};

/**
 * Holds all of the valid requests
 */
extern RequestLink requests[];

/**
 * Parse the request and write the arguemnts to args
 * @param parse_line the input request to read from
 * @param args the arguments parsed from the request
 * @return a pointer to function that should be called
 */
SHFP parse_request (char* parse_line, StringVector* args);

/**
 * HTTP request to download file
 * @param conn the connection that holds the request
 * @param args [path] [HTTP/1.0 or 1.1]
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t GET (Connection* conn, char** args, int start, int argc);

/**
 * Install a packages to the bounded host
 * @param conn the connection that holds the request
 * @param args [emerge argument[s]]
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t INSTALL (Connection* conn, char** args, int start, int argc);

/* SRV Configure requests */

//response_t SRV_DEVCREATE (Connection* conn, char** args, int start);

/**
 * Edit an existing Host
 * 
 * ARGUMENTS ARE SEPARATED BY NEW LINES!
 * @param conn the connection that holds the request
 * @param args [argc of ETC] [ID] [HOSTNAME] [PROFILE] [CHOST] [CFLAGS] [USE] [ETC]
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_EDIT (Connection* conn, char** args, int start, int argc);

/**
 * Bind the connections ip address to the specified Host*
 * @param conn the connection that holds the request
 * @param args [HOST_ID]
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_ACTIVATE (Connection* conn, char** args, int start, int argc);

/**
 * Delete a host (currently all users can do this)
 * 
 * This will be changed
 * @param conn the connection that holds the request
 * @param args [HOST_ID]
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_HOSTREMOVE (Connection* conn, char** args, int start, int argc);

/* SRV Utility request */

/**
 * Mount all the directories required to chroot() to Host
 * @param conn the connection that holds the request
 * @param args (none)
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_MNTCHROOT (Connection* conn, char** args, int start, int argc);

/* SRV Metadata requests */

/**
 * Returns metadata of the Host selected
 * @param conn the connection that holds the request
 * @param args [HOST_ID]
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_GETHOST (Connection* conn, char** args, int start, int argc);

/**
 * Returns an int (host count) followed by a list of the created hosts
 * 
 * All of these are newline separated
 * @param conn the connection that holds the request
 * @param args (none)
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_GETHOSTS (Connection* conn, char** args, int start, int argc);

/**
 * Returns the ID of the bounded host of the IP from the current request
 * @param conn the connection that holds the request
 * @param args (none)
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_GETACTIVE (Connection* conn, char** args, int start, int argc);

/**
 * Returns information outputed from 'lscpu' of the build server
 * @param conn the connection that holds the request
 * @param args (none)
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_GETSPEC (Connection* conn, char** args, int start, int argc);

/**
 * Returns a list of all the availiable templates
 * @param conn the connection that holds the request
 * @param args (none)
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_GETTEMPPLATES (Connection* conn, char** args, int start, int argc);

/**
 * @brief Create/initilize a new template
 * 
 * Commands include the following
 *  - DOWNLOAD: download the stage3
 *  - EXTRACT: extract stage3 to dest_dir
 *  - ALL: DOWNLOAD and extract
 *  - anything: this will specify the path to 
 *      the stage3 if EXTRACT is the only command used.
 *      This must be put BEFORE the EXTRACT command
 * @param conn the connection that holds the request
 * @param args the template id of the new template (look in stage.c)
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_TEMPLATE (Connection* conn, char** args, int start, int argc);

/**
 * Saves the current server to the config
 * You can use this whenever, its perfectly safe
 * @param conn the connection that holds the request
 * @param args (none)
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_SAVE (Connection* conn, char** args, int start, int argc);

/**
 * End the server's main loop
 * @param conn the connection that holds the request
 * @param args (none)
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t EXIT (Connection* conn, char** args, int start, int argc);

/* SRV Kernel request (unimplmented) */

#endif
