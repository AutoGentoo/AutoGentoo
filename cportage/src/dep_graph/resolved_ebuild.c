//
// Created by atuser on 12/29/19.
//

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include "resolved_ebuild.h"
#include <stdarg.h>
#include "../package.h"
#include "../portage.h"
#include "resolve_request.h"
#include <string.h>

char* prv_asrprintf(char* fmt, ...) {
	char* __dest_tm = NULL;
	
	va_list arg;
	va_start(arg, fmt);
	vasprintf(&__dest_tm, fmt, arg);
	va_end(arg);
	
	return __dest_tm;
}

ResolvedEbuild* re_new(Emerge* environ, Ebuild* ebuild, ResolvedSlot* parent) {
	ResolvedEbuild* out = malloc(sizeof(ResolvedEbuild));
	out->environ = environ;
	out->parent = parent;
	
	out->target = ebuild;
	
	out->use = NULL;
	out->explicit = NULL;
	
	out->bdepend = vector_new(VECTOR_REMOVE | VECTOR_UNORDERED);
	out->depend = vector_new(VECTOR_REMOVE | VECTOR_UNORDERED);
	out->rdepend = vector_new(VECTOR_REMOVE | VECTOR_UNORDERED);
	out->pdepend = vector_new(VECTOR_REMOVE | VECTOR_UNORDERED);
	
	return out;
}

char** re_get_env(ResolvedEbuild* re) {
	char* pvr = NULL;
	
	if (re->target->revision > 0)
		pvr = prv_asrprintf("PVR=%s-r%d", re->target->version->full_version, re->target->revision);
	else
		pvr = prv_asrprintf("PVR=%s", re->target->version->full_version);
	
	char* env_temp[] = {
			strdup("PORTAGE_BIN_PATH="PORTAGE_BIN_PATH),
			strdup("PORTAGE_BUILDDIR="PORTAGE_BUILDDIR),
			strdup("PORTAGE_TMP_DIR="PORTAGE_TMP_DIR),
			strdup("WORKDIR="WORKDIR),
			strdup("EBUILD_PHASE=setup"),
			strdup("EMERGE_FROM=ebuild"),
			strdup("T="T),
			strdup("D="D),
			strdup("HOME="HOME),
			strdup("_IN_INSTALL_QA_CHECK=1"),
			strdup("PORTAGE_GRPNAME="PORTAGE_GROUPNAME),
			strdup("EAPI="PORTAGE_EAPI),
			prv_asrprintf("EBUILD=%s", re->target->ebuild),
			prv_asrprintf("CATEGORY=%s", re->target->category),
			prv_asrprintf("P=%s-%s", re->target->name, re->target->version->full_version),
			prv_asrprintf("PN=%s", re->target->name),
			prv_asrprintf("PV=%s", re->target->version->full_version),
			prv_asrprintf("PORTAGE_ECLASS_LOCATIONS=%s", re->target->parent->parent->location),
			prv_asrprintf("CACHE_DIR="PORTAGE_CACHE"/%s", re->target->category),
			prv_asrprintf("CACHE_FILE=%s", re->target->cache_file),
			prv_asrprintf("MD5_HASH=%s", re->target->ebuild_md5),
			prv_asrprintf("PF=%s", re->target->pf),
			pvr,
			NULL
	};
	
	int env_n = sizeof(env_temp) / sizeof(env_temp[0]);
	char** env = malloc(sizeof(char*) * env_n);
	
	for (int i = 0; i < env_n; i++)
		env[i] = env_temp[i];
	
	return env;
}

void re_free(ResolvedEbuild* re) {
	use_free(re->use);
	use_free(re->explicit);
	
	Vector* to_free[] = {
			re->bdepend,
			re->depend,
			re->rdepend,
			re->pdepend
	};
	for (int i = 0; i < 4; i++) {
		vector_foreach(to_free[i], (void (*)(void*)) rr_free);
		vector_free(to_free[i]);
	}
	
	if (re->parent->delete_on_re_free) {
		re->parent->target = NULL;
		rs_free(re->parent);
	}
	
	free(re);
}