//
// Created by atuser on 4/28/19.
//

#define _GNU_SOURCE

#include <archive.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "compress.h"
#include "portage_log.h"
#include "portage.h"
#include <string.h>

FILE* fread_archive(char* path, int dirfd, char* verify_sha) {
	char* buffer_file;
	asprintf(&buffer_file, "%s.cportage.decomp", path);
	FILE* fp;
	if (verify_sha) {
		int out_fd = openat(dirfd, buffer_file, O_RDONLY);
		if (out_fd > 0) {
			sha_hash fd_hash;
			portage_get_hash_fd(&fd_hash, out_fd, EVP_sha256());
			if (strcmp(verify_sha, (char*)fd_hash) != 0)
				close(out_fd);
			else {
				fp = fdopen(out_fd, "r");
				rewind(fp);
				free(buffer_file);
				return fp;
			}
		}
	}
	
	struct archive* a = archive_read_new();
	struct archive_entry* ae;
	archive_read_support_format_all(a);
	archive_read_support_filter_gzip(a);
	archive_read_support_format_raw(a);
	
	int fd = openat(dirfd, path, O_RDONLY | O_CLOEXEC);
	if (fd == -1) {
		plog_error("Failed to open file %s for reading", path);
		archive_free(a);
		return NULL;
	}
	
	if (archive_read_open_fd(a, fd, 16384)) {
		plog_warn("Failed to open archive %s for reading", path);
		plog_warn("libarchive -- %s", archive_error_string(a));
		archive_read_free(a);
		return NULL;
	}
	
	if (archive_read_next_header(a, &ae)) {
		plog_error("Failed to read header from %s", path);
		archive_read_free(a);
		archive_read_close(a);
		return NULL;
	}
	
	int b_fd = openat(dirfd, buffer_file, O_RDWR | O_CREAT | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if (b_fd < 0) {
		return NULL;
	}
	fp = fdopen(b_fd, "w+");
	
	if (!fp) {
		plog_error("Failed to open buffer file %s", buffer_file);
		free(buffer_file);
		archive_read_close(a);
		archive_read_free(a);
		return NULL;
	}
	free(buffer_file);
	
	size_t size;
	char buffer[4096];
	for (;;) {
		size = archive_read_data(a, buffer, 4096);
		if (size < 0) {
			plog_error("Failed to read from %s", path);
			archive_read_free(a);
			archive_read_close(a);
			return NULL;
		}
		if (size == 0)
			break;
		fwrite(buffer, size, 1, fp);
	}
	
	archive_read_close(a);
	archive_read_free(a);
	
	rewind(fp);
	return fp;
}