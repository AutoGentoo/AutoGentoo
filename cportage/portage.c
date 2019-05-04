//
// Created by atuser on 4/23/19.
//

#include <sys/stat.h>
#include <errno.h>
#include "portage.h"
#include "portage_log.h"
#include <string.h>
#include <fcntl.h>

int portage_get_hash_fd(SHA_HASH target, int fd, const EVP_MD* algorithm) {
	FILE* hash = fdopen(fd, "r");
	
	EVP_MD_CTX mdctx;
	unsigned int md_len;
	EVP_MD_CTX_init(&mdctx);
	EVP_DigestInit_ex(&mdctx, algorithm, NULL);
	
	char chunk[64];
	ssize_t current_bytes = 0;
	while ((current_bytes = fread(chunk, 64, 1, hash)) > 0) {
		EVP_DigestUpdate(&mdctx, chunk, current_bytes);
	}
	
	EVP_DigestFinal_ex(&mdctx, target, &md_len);
	EVP_MD_CTX_cleanup(&mdctx);
	
	return (int)md_len;
}

int portage_get_hash(SHA_HASH target, char* path, const EVP_MD* algorithm) {
	struct stat path_stat;
	if (stat(path, &path_stat) != 0) {
		plog_warn("Could not get hash - %s [%d]", strerror(errno));
		return -1;
	}
	
	int fd = open(path, O_RDONLY);
	return portage_get_hash_fd(target, fd, algorithm);
}