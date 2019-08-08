//
// Created by atuser on 6/23/19.
//

#define _GNU_SOURCE

#include <autogentoo/crypt.h>
#include "autogentoo/host_environment.h"
#include <string.h>
#include <stdio.h>

HostEnvironment* host_environment_new(Host* parent) {
	HostEnvironment* out = malloc(sizeof(HostEnvironment));
	
	out->cflags = NULL;
	out->cxxflags = NULL;
	out->distdir = NULL;
	out->extra = small_map_new(5);
	out->lc_messages = NULL;
	
	out->package_accept_keywords = vector_new(sizeof(PortageEntry*), VECTOR_REMOVE | VECTOR_ORDERED);
	out->package_env = vector_new(sizeof(PortageEntry*), VECTOR_REMOVE | VECTOR_ORDERED);
	out->package_use = vector_new(sizeof(PortageEntry*), VECTOR_REMOVE | VECTOR_ORDERED);
	out->package_mask = string_vector_new();
	
	out->pkgdir = NULL;
	out->portage_logdir = NULL;
	out->portage_tmpdir = NULL;
	out->portdir = NULL;
	out->use = NULL;
	
	out->make_conf_sha = NULL;
	out->package_accept_keywords_sha = NULL;
	out->package_env_sha = NULL;
	out->package_use_sha = NULL;
	out->package_mask_sha = NULL;
	
	return out;
}

void host_environment_backup(Host* target) {
	struct {
		char* old_sha;
		char* rel_filepath;
	} sha_checks[] =  {
		{target->environment->make_conf_sha, "make.conf"},
		{target->environment->package_accept_keywords_sha, "package.accept_keywords"},
		{target->environment->package_env_sha, "package.env"},
		{target->environment->package_use_sha, "package.use"},
		{target->environment->package_mask_sha, "package.mask"}
	};
	
	for (int i = 0; i < (int)(sizeof(sha_checks) / sizeof(sha_checks[0])); i++) {
		if (!sha_checks[i].old_sha) {
			lwarning("Skipping hash check for %s (%s)", sha_checks[i].rel_filepath, target->id);
			continue;
		}
		
		char* filepath = host_path(target, "/etc/portage/%s", sha_checks[i].rel_filepath);
		
		FILE* fp = fopen(filepath, "r");
		if (!fp) {
			lerror("Could not open: %s", filepath);
			free(filepath);
			continue;
		}
		
		char* to_check = fread_sha256(fp);
		
		if (strcmp(sha_checks[i].old_sha, to_check) == 0)
			linfo("Hash check passed for %s (%s)", sha_checks[i].rel_filepath, target->id);
		else {
			int b;
			size_t bytes_read = 0;
			
			char* backup_path = NULL;
			asprintf(&backup_path, "%s.%ld.backup", filepath, time(NULL));
			FILE* backup = fopen(backup_path, "w+");
			
			while ((bytes_read = fread(&b, sizeof(int), 1, fp)) > 0)
				fwrite(&b, bytes_read, 1, backup);
			
			lwarning("Hash for %s (%s) did not match", sha_checks[i].rel_filepath, target->id);
			lwarning("Backed up old file to %s", backup_path);
			
			fflush(backup);
			fclose(backup);
			free(backup_path);
		}
		
		fclose(fp);
		free(filepath);
		free(to_check);
	}
}

int host_environment_write(Host* target) {
	host_environment_backup(target);
	host_environment_write_make_conf(target);
	
	
	return 0;
}

#define LINEWRITE(str...) {fprintf(fp, str); fwrite("\n", 1, 1, fp);}

void host_environment_write_make_conf(Host* target) {
	char* filepath = host_path(target, "/etc/portage/make.conf");
	FILE* fp = fopen(filepath, "w+");
	
	LINEWRITE("# This file is automatically generated by the AutoGentoo service")
	LINEWRITE("# Contents changed here will be backed up in this directly")
	LINEWRITE("# To make persistant changes, edit this Host's config on autogentoo.org")
	LINEWRITE("")
	LINEWRITE("# make.conf for %s", target->id)
	LINEWRITE("")
	LINEWRITE("# CFLAGS will be passed to every program compiled with GCC (almost everything).")
	LINEWRITE("# CFLAGS can be to optimize the runtime of the generated binaries")
	LINEWRITE("# NOTE: some CFLAGS may speed up the system on specific CPUs and not run on others")
	LINEWRITE("# Refer to https://wiki.gentoo.org/wiki/GCC_optimization for details on what to write here")
	LINEWRITE("# NEVER USE '-march=native' unless you are sure the compile server has the same cpu as the client")
	LINEWRITE("CFLAGS=\"%s\"", target->environment->cflags)
	
	LINEWRITE("")
	LINEWRITE("# CXXFLAGS are the same as CFLAGS except passed only to C++ compiles")
	LINEWRITE("# Recommended: CXXFLAGS=\"${CFLAGS}\"")
	LINEWRITE("CXXFLAGS=\"%s\"", target->environment->cxxflags)
	
	LINEWRITE("")
	LINEWRITE("# USE flags will add or remove certain features of installed packages")
	LINEWRITE("# The USE variable in this file will apply globally to every package")
	LINEWRITE("# To change a use flags for one package, use package.use")
	LINEWRITE("USE=\"%s\"", target->environment->use)
	
	LINEWRITE("")
	LINEWRITE("# The following entries were appended by the user")
	
	for (int i = 0; i < (int)target->environment->extra->n; i++) {
		SmallMap_key* c_key = *(SmallMap_key**)vector_get(target->environment->extra, i);
		LINEWRITE("%s=\"%s\"", c_key->key, (char*)c_key->data_ptr)
	}
	
	LINEWRITE("")
	LINEWRITE("")
	LINEWRITE("# The following should NOT be changed")
	LINEWRITE("# Build directory")
	LINEWRITE("PORTAGE_TMPDIR=\"%s\"", target->environment->portage_tmpdir)
	LINEWRITE("# ebuild portage tree (shared)")
	LINEWRITE("PORTDIR=\"%s\"", target->environment->portdir)
	LINEWRITE("# Distfile directory (shared)")
	LINEWRITE("DISTDIR=\"%s\"", target->environment->distdir)
	LINEWRITE("# Package binary directory")
	LINEWRITE("PKGDIR=\"%s\"", target->environment->pkgdir)
	LINEWRITE("# Build log directory")
	LINEWRITE("PORTAGE_LOGDIR=\"%s\"", target->environment->portage_logdir)
	LINEWRITE("# Log message language")
	LINEWRITE("LC_MESSAGES=\"%s\"", target->environment->lc_messages)
	LINEWRITE("")
	
	if (target->environment->make_conf_sha)
		free(target->environment->make_conf_sha);
	
	target->environment->make_conf_sha = fread_sha256(fp);
	fclose(fp);
	free(filepath);
}

/*
int host_environment_convert_portagedir(Host* target, char* filename) {
	char* dirpath = host_path(target, "/etc/portage/%s", filename);
	struct stat buf;
	
	if (stat(dirpath, &buf) == -1) {
		lwarning("File %s does not exist");
		free(dirpath);
		return 0;
	}
	
	if ((buf.st_mode & S_IFMT) == S_IFDIR) {
		struct dirent *de;  // Pointer for directory entry
		DIR* dr = opendir(dirpath);
		
		if (!dr) {
			lerror("Failed to open directory: %s", dirpath);
			return 1;
		}
		
		char* buf_path = host_path(target, "/etc/portage/%s.%d.temp", filename, time(NULL));
		FILE* fp = fopen(buf_path, "w+");
		
		while ((de = readdir(dr)) != NULL) {
			char* cpfile = NULL;
			asprintf(&cpfile, "%s/%s", dirpath, de->d_name);
			
			FILE* cpfp = fopen(cpfile, "rb");
			
			LINEWRITE("# FILE: %s", cpfile)
			int cpbuf;
			size_t bytes_read = 0;
			
			while ((bytes_read = fread(&cpbuf, sizeof(int), 1, cpfp)) > 0)
				fwrite(&cpbuf, bytes_read, 1, fp);
			
			LINEWRITE("# End of file %s", cpfile)
			LINEWRITE("")
			
			fclose(cpfp);
			free(cpfile);
		}
		
		closedir(dr);
		
		char* remove_command = NULL;
		asprintf(&remove_command, "rm -rf %s", dirpath);
		
		linfo("Removing directory %s", dirpath);
		int res = system(dirpath);
		if (res != 0) {
			lerror("Failed to remove directory: %s", dirpath);
			lerror("Error [%d]: %s", res, strerror(res));
			free(dirpath);
			free(remove_command);
			free(buf_path);
			fclose(fp);
			return 1;
		}
		
		FILE* fp_out = fopen(dirpath, "w+");
		fclose(fp);
		fp = fopen(buf_path, "rb");
		
		int cpbuf;
		size_t bytes_read = 0;
		
		while ((bytes_read = fread(&cpbuf, sizeof(int), 1, fp)) > 0)
			fwrite(&cpbuf, bytes_read, 1, fp_out);
		
		fclose(fp);
		fclose(fp_out);
		
		// Remove the temp file
		remove(buf_path);
		
		free(buf_path);
		free(remove_command);
		free(dirpath);
	}
	
	free(dirpath);
	return 0;
}
*/

void host_environment_write_conf(Host* target, Vector* content, char* file) {
	
	
	char* filepath = host_path(target, "/etc/portage/%s", file);
	FILE* fp = fopen(filepath, "w+");
}