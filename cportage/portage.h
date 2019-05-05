//
// Created by atuser on 4/23/19.
//

#ifndef AUTOGENTOO_PORTAGE_H
#define AUTOGENTOO_PORTAGE_H

#include <autogentoo/hacksaw/map.h>
#include <openssl/evp.h>
#include "constants.h"

typedef struct __Repository Repository;
typedef struct __UseSelector UseSelector;

struct __Repository {
	Map* packages;
	char* name;
	char* root_path;
	char* conf_dir;
	
	sha_hash package_accept_keywords;
	sha_hash package_use;
	sha_hash make_conf;
};


Repository* repository_new (char* name, char* root, char* conf_dir);

int portage_get_hash(sha_hash* target, char* path, const EVP_MD* algorithm);
int portage_get_hash_fd(sha_hash* target, int fd, const EVP_MD* algorithm);


#endif //AUTOGENTOO_PORTAGE_H
