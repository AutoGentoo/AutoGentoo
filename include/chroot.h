#ifndef __AUTOGENTOO_CHROOT_H__
#define __AUTOGENTOO_CHROOT_H__

typedef struct __Chroot Chroot;
typedef struct __ChrootMount ChrootMount;

typedef enum {
    NO_RECURSIVE,
    RECURSIVE
} mount_t;

typedef enum {
    FAILED = 0x0,
    PORTAGE = 0x1,
    SYS = 0x2,
    DEV = 0x4,
    PROC = 0x8
} mounts_t;

struct __Chroot {
    Host* parent;
    Vector* mounts;
    mounts_t status;
};

struct __ChrootMount {
    char* src;
    char* dest;
    char* type; // NULL for default
    mount_t opts;
}

response_t enter (Chroot* chroot, (response_t*)(void*));
Chroot* chroot_new (Host* parent);
mounts_t chroot_mount (Chroot* chroot);

#endif