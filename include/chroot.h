#ifndef __AUTOGENTOO_CHROOT_H__
#define __AUTOGENTOO_CHROOT_H__

typedef struct __Chroot Chroot;
typedef struct __ChrootMount ChrootMount;

typedef enum {
    
} mount_t;

struct __Chroot {
    
};

struct __ChrootMount {
    char* src;
    char* dest;
    char* type; // NULL for default
    mount_t opts;
}

#endif