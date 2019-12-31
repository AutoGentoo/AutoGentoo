//
// Created by atuser on 12/31/19.
//

#define _GNU_SOURCE

#include <autogentoo/hacksaw/string_vector.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "cache.h"
#include "package.h"
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include "portage.h"

char* prv_cache_variable(char** dest, char* fmt, ...) {
	if (*dest)
		free(*dest);
	
	va_list arg;
	va_start(arg, fmt);
	vasprintf(dest, fmt, arg);
	va_end(arg);
	
	return *dest;
}

int cache_generate(Ebuild* target) {
	if (!target)
		return 0;
	
	StringVector* env = string_vector_new();
	string_vector_add(env, "PORTAGE_BIN_PATH="PORTAGE_BIN_PATH);
	string_vector_add(env, "PORTAGE_TMP_DIR="PORTAGE_TMP_DIR);
	string_vector_add(env, "PORTAGE_BUILDDIR="PORTAGE_BUILDDIR);
	string_vector_add(env, "WORKDIR="WORKDIR);
	string_vector_add(env, "T="T);
	string_vector_add(env, "D="D);
	string_vector_add(env, "HOME="HOME);
	
	char* temp = NULL;
	string_vector_add(env, prv_cache_variable(&temp, "CATEGORY=%s", target->category));
	string_vector_add(env, prv_cache_variable(&temp, "P=%s-%s", target->parent->name, target->version->full_version));
	string_vector_add(env, prv_cache_variable(&temp, "PN=%s", target->parent->name));
	string_vector_add(env, prv_cache_variable(&temp, "PV=%s", target->version->full_version));
	
	if (target->revision > 0) {
		string_vector_add(env, prv_cache_variable(&temp, "PVR=%s-r%d", target->version->full_version, target->revision));
		string_vector_add(env, prv_cache_variable(&temp, "PF=%s-%s-r%d", target->parent->name, target->version->full_version, target->revision));
	}
	else {
		string_vector_add(env, prv_cache_variable(&temp, "PVR=%s", target->version->full_version));
		string_vector_add(env, prv_cache_variable(&temp, "PF=%s-%s", target->parent->name, target->version->full_version));
	}
	
	string_vector_add(env, NULL);
	free(temp);
	
	pid_t env_pid = fork();
	
	
	if (env_pid == 0) {
		char** env_ptr = (char**)env->ptr;
		free(env);
		
		temp = NULL;
		
		char* ebuild_file = NULL;
		if (target->revision > 0)
			asprintf(&ebuild_file, "%s/%s/%s-%s-r%d.ebuild", target->parent->parent->location, target->category, target->parent->name, target->version->full_version, target->revision);
		else
			asprintf(&ebuild_file, "%s/%s/%s-%s.ebuild", target->parent->parent->location, target->category, target->parent->name, target->version->full_version);
		
		char* args[] = {
				ebuild_file,
				"setup"
		};
		execve(PORTAGE_BIN_PATH"/ebuild.sh", args, env_ptr);
	}
	
	int env_ret = 0;
	waitpid(env_pid, &env_ret, 0);
	
	return 1;
}