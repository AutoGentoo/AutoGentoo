#ifndef __AUTOGENTOO_DOWNLOAD_H__
#define __AUTOGENTOO_DOWNLOAD_H__

typedef struct __Download Download;

typedef enum {
    NO_PROGRESS,
    SHOW_PROGRESS
} down_progress;

struct __Download {
    char* url;
    size_t size;
    size_t progress;
};

response_t download (char* url, down_progress p);

#endif