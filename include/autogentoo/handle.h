#ifndef __AUTOGENTOO_HANDLE_H__
#define __AUTOGENTOO_HANDLE_H__

#include "server.h"
#include "response.h"
#include "request.h"

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
//SHFP parse_request(char* parse_line, StringVector* args);

/**
 * HTTP request to download file
 * @param conn the connection that holds the request
 * @param args [path] [HTTP/1.0 or 1.1]
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t GET(Connection* conn, HTTPRequest req);

/**
 * Install a packages to the bounded host
 * @param conn the connection that holds the request
 * @param args [emerge argument[s]]
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t INSTALL(Request* request);

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
response_t SRV_EDIT(Request* request);

/**
 * Bind the connections ip address to the specified Host*
 * @param conn the connection that holds the request
 * @param args [HOST_ID]
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_ACTIVATE(Request* request);

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
response_t SRV_HOSTREMOVE(Request* request);

/* SRV Utility request */

/**
 * Mount all the directories required to chroot() to Host
 * @param conn the connection that holds the request
 * @param args (none)
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_MNTCHROOT(Request* request);

/* SRV Metadata requests */

/**
 * Returns metadata of the Host selected
 * @param conn the connection that holds the request
 * @param args [HOST_ID]
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_GETHOST(Request* request);

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
response_t SRV_GETHOSTS(Request* request);

/**
 * Returns the ID of the bounded host of the IP from the current request
 * @param conn the connection that holds the request
 * @param args (none)
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_GETACTIVE(Request* request);

/**
 * Returns information outputed from 'lscpu' of the build server
 * @param conn the connection that holds the request
 * @param args (none)
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_GETSPEC(Request* request);

/**
 * Returns a list of all the availiable templates
 * @param conn the connection that holds the request
 * @param args (none)
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_GETTEMPLATES(Request* request);

/**
 * Create a new template given binary data
 * Should only be called from client.py
 * @param conn the connection that holds the request
 * @param args binary data
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_TEMPLATE_CREATE(Request* request);

/**
 * Creates a new stage from an existing template given a template id
 * @param conn the connection that holds the request
 * @param args the template id of the new template (look in stage.c)
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_STAGE_NEW(Request* request);

/**
 * @brief Create/initilize a new stage
 * 
 * Commands include the following
 *  - DOWNLOAD: download the stage3
 *  - EXTRACT: extract stage3 to dest_dir
 *  - ALL: DOWNLOAD and extract
 *  - anything: this will specify the path to 
 *      the stage3 if EXTRACT is the only command used.
 *      This must be put BEFORE the EXTRACT command
 * @param conn the connection that holds the request
 * @param args the id of the staged template
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_STAGE(Request* request);

/**
 * Returns a list of the currently staged HostTemplates
 * @param conn the connection that holds the request
 * @param args (none)
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_GETSTAGED(Request* request);

/**
 * Returns a info about the selected staged HostTemplate
 * @param conn the connection that holds the request
 * @param args host_id
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_GETSTAGE(Request* request);

/**
 * Handoff a HostTemplate to Host
 * @param conn the connection that holds the request
 * @param args host_id
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_HANDOFF(Request* request);

/**
 * Saves the current server to the config
 * You can use this whenever, its perfectly safe
 * @param conn the connection that holds the request
 * @param args (none)
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_SAVE(Request* request);

/**
 * End the server's main loop
 * @param conn the connection that holds the request
 * @param args (none)
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t EXIT(Request* request);

/**
 * Write the Server struct back to the client
 * @param conn the connection that holds the request
 * @param args (none)
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t BIN_SERVER(Request* request);

/**
 * Links host_environ directory and write configs
 * useful for regening a corrupt build environ
 * @param conn the connection that holds the request
 * @param args (none)
 * @param start Start index to read from request
 * @param argc the argument count in args
 * @return HTTP standard codes
 */
response_t SRV_HOSTWRITE(Request* request);

//response_t SRV_HOSTUPLOAD(Request* request);

/* SRV Kernel request (unimplmented) */



#endif
