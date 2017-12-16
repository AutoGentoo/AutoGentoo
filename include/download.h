#ifndef __AUTOGENTOO_DOWNLOAD_H__
#define __AUTOGENTOO_DOWNLOAD_H__

#include <response.h>

typedef enum {
    NO_PROGRESS,
    SHOW_PROGRESS
} down_progress;

response_t download (char* url, char* dest, down_progress p);

#endif