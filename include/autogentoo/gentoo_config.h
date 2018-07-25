//
// Created by atuser on 7/24/18.
//

#ifndef AUTOGENTOO_GENTOO_CONFIG_H
#define AUTOGENTOO_GENTOO_CONFIG_H

typedef struct __GentooConfig GentooConfig;
typedef struct __GentooConfigEntry GentooConfigEntry;
typedef struct __GentooConfigFlag GentooConfigFlag

typedef enum {
	GENTOO_CONFIG_FLAG_OFF,
	GENTOO_CONFIG_FLAG_ON,
} gconfig_flag_status_t;

struct __GentooConfig {

};

struct __GentooConfigEntry {
	char* atom_selector;
};

struct __GentooConfigFlag {
	char* name;
	gconfig_flag_status_t status;
};

#endif //AUTOGENTOO_GENTOO_CONFIG_H
