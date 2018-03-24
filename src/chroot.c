#include <autogentoo/chroot.h>
#include <sys/mount.h>
#include <string.h>
#include <stdlib.h>

#ifdef __APPLE__

void prv_get_mounted (StringVector* dest) {
	lerror ("chroot/mounting not supported on MACOSX");
}

#elif __linux__

#include <mntent.h>

void prv_get_mounted(StringVector* dest) {
	struct mntent* fs;
	FILE* fp = setmntent("/proc/mounts", "r");
	if (fp == NULL) {
		lerror("Could not open /proc/mounts!");
		exit(1);
	}
	
	while ((fs = getmntent(fp)) != NULL)
		string_vector_add(dest, fs->mnt_dir);
	endmntent(fp);
}

#endif

#ifndef __linux__
enum {
	MS_BIND = 0x0,
	MS_REC = 0x0,
	MS_MGC_VAL = 0x0
};

#endif

response_t chroot_mount(Host* host) {
	ChrootMount to_mount[] = {
			{"/usr/portage", "usr/portage", NULL, MS_BIND},
			{"/dev",         "dev",         NULL, MS_BIND | MS_REC},
			{"/sys",         "sys",         NULL, MS_BIND | MS_REC},
			{"/proc",        "proc", "proc",      MS_MGC_VAL}
	};
	
	StringVector* mounted = string_vector_new();
	prv_get_mounted(mounted);
	
	if (mounted->n == 0) {
		lerror("System could not detect mounted partitions!");
		return INTERNAL_ERROR;
	}
	
	char* new_root;
	host_get_path(host, &new_root);
	
	int i;
	for (i = 0; i != sizeof(to_mount) / sizeof(to_mount[0]); i++) {
		char* dest_temp;
		ChrootMount mnt = to_mount[i];
		
		dest_temp = malloc(strlen(new_root) + strlen(mnt.dest) + 2);
		sprintf(dest_temp, "%s/%s", new_root, mnt.dest);
		fix_path(dest_temp);
		
		if (string_find(mounted->ptr, dest_temp, (size_t) mounted->n) == -1) {
#ifdef __linux__
			if (mount(mnt.src, dest_temp, mnt.type, mnt.opts, NULL) == -1) {
				lerror("Failed to mount %s", dest_temp);
				string_vector_free(mounted);
				free(dest_temp);
				host->chroot_status = CHR_NOT_MOUNTED;
				return INTERNAL_ERROR;
			}
			lwarning("mounting %s to %s", mnt.src, dest_temp);
#else
			return INTERNAL_ERROR;
#endif
		}
		free(dest_temp);
	}
	
	free(new_root);
	string_vector_free(mounted);
	host->chroot_status = CHR_MOUNTED;
	return OK;
}
