//
// Created by atuser on 1/5/18.
//

#ifndef AUTOGENTOO_ABS_AABS_H
#define AUTOGENTOO_ABS_AABS_H

#include <sys/types.h>
#include <hacksaw/tools.h>


typedef Vector aabs_vector_t;
typedef StringVector aabs_svec_t;
typedef SmallMap aabs_small_map_t;
typedef Map aabs_map_t;
typedef LinkedNode aabs_node_t;

typedef struct __aabs_file_t aabs_file_t;
typedef struct __aabs_filelist_t aabs_filelist_t;

typedef int64_t aabs_int64_t;
typedef aabs_int64_t aabs_time_t;

typedef enum {
    AABS_ERR_MEMORY = 1,
    AABS_ERR_NOT_A_FILE,
    AABS_ERR_NOT_A_DIRECTORY,
    AABS_ERR_DISK_SPACE,
    
    /* Function calls */
    AABS_ERR_NULL_PTR,
    
    /* Database */
    AABS_ERR_DB_CREATE,
    AABS_ERR_DB_READ,
    AABS_ERR_DB_NOT_FOUND,
    
    /* Internet */
    AABS_ERR_NET_NOT_FOUND,
    AABS_ERR_NET_SERVER_INVALID,
    
    /* Package */
    AABS_ERR_PKG_NOT_FOUND,
    AABS_ERR_PKG_INVALID,
    AABS_ERR_PKG_INVALID_CHECKSUM,
    AABS_ERR_PKG_INVALID_SIG
} aabs_error_t;

/* The error status of the last failure */
static aabs_error_t aabs_errno;

struct __aabs_file_t {
    char* name;
    off_t size;
    mode_t mode;
};

struct __aabs_filelist_t {
    size_t count;
    aabs_file_t* files;
};

#endif //AUTOGENTOO_ABS_AABS_H
