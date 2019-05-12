//
// Created by atuser on 5/12/19.
//

#include "directory.h"
#include <errno.h>
#include "portage_log.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

FPNode* open_directory_stat(mode_t st_mode, int buf_fd, char* path) {
	FPNode* files = NULL;
	FPNode* temp;
	if (S_ISDIR(st_mode)) {
		DIR *d;
		struct dirent *dir;
		d = fdopendir(buf_fd);
		if (d) {
			while ((dir = readdir(d)) != NULL) {
				int fd_temp = openat(buf_fd, dir->d_name, O_RDONLY);
				if (fd_temp < 0)
					continue;
				struct stat c_st;
				fstat(fd_temp, &c_st);
				
				temp = malloc(sizeof(FPNode));
				temp->filename = strdup(dir->d_name);
				temp->fp = NULL;
				temp->dirfd = -1;
				
				if (S_ISREG(c_st.st_mode)) {
					temp->fp = fdopen(fd_temp, "r");
					temp->type = FP_NODE_FILE;
				}
				else if (S_ISDIR(c_st.st_mode)) {
					temp->dirfd = fd_temp;
					temp->type = FP_NODE_DIR;
				}
				
				temp->next = files;
				files = temp;
			}
			closedir(d);
		}
	}
	else if (S_ISREG(st_mode)) {
		files = malloc(sizeof(FPNode));
		files->fp = fdopen(buf_fd, "r");
		files->dirfd = -1;
		files->filename = strdup(path);
		files->type = FP_NODE_FILE;
		files->next = NULL;
	}
	
	return files;
}

FPNode* open_directory(char* path) {
	struct stat st;
	if (stat(path, &st) == -1) {
		if (errno == ENOENT)
			return NULL;
		plog_error("Failed to open %s", path);
		return NULL;
	}
	
	int buf_fd = open(path, O_RDONLY);
	
	return open_directory_stat(st.st_mode, buf_fd, path);
}

FPNode* open_directory_at(int parent_dir, char* path) {
	if (parent_dir < 0)
		return open_directory(path);
	
	struct stat st;
	if (fstatat(parent_dir, path, &st, 0) == -1) {
		if (errno == ENOENT)
			return NULL;
		plog_error("Failed to open %s", path);
		return NULL;
	}
	
	int buf_fd = openat(parent_dir, path, O_RDONLY);
	return open_directory_stat(st.st_mode, buf_fd, path);
}