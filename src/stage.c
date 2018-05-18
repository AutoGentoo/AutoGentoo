#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <autogentoo/stage.h>
#include <autogentoo/command.h>

/*
 * Arguments (Everything after chost is optional)
 * [id] [portage-arch] [cflags] [chost] [extras]
 */

HostTemplate host_templates_init[] = {
		/*
		{"alpha", "alpha", "-mieee -pipe -O2 -mcpu=ev4", "alpha-unknown-linux-gnu", 1, {"/space/catalyst/portage", PORTDIR}},
		 */
		{"amd64",         "amd64", "-O2 -pipe", "x86_64-pc-linux-gnu", 1, {"CPU_FLAGS_X86=\"mmx sse sse2\"", OTHER}},
		{"amd64-systemd", "amd64", "-O2 -pipe", "x86_64-pc-linux-gnu", 1, {"CPU_FLAGS_X86=\"mmx sse sse2\"", OTHER}},
		/*
		{"armv4tl", "arm"},
		{"armv5tel", "arm"},
		{"armv6j", "arm"},
		{"armv6j_hardfp", "arm"},
		{"armv7a", "arm"},
		{"armv7a_hardfp", "arm", "-O2 -pipe -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=hard", "armv7a-hardfloat-linux-gnueabi"},
		{"arm64", "arm", "-O2 -pipe", "aarch64-unknown-linux-gnu"},
		{"hppa", "hppa", "-O2 -pipe -march=1.1", "hppa1.1-unknown-linux-gnu", "CXXFLAGS=\"-O2 -pipe\"", CXXFLAGS},
		*/
};

HostTemplate* prv_host_template_alloc (HostTemplate* t) {
	HostTemplate* out = malloc(sizeof(HostTemplate));
	
	out->id = strdup(t->id);
	out->arch = strdup(t->arch);
	out->cflags = strdup(t->cflags);
	out->chost = strdup(t->chost);
	out->extra_c = t->extra_c;
	
	int i;
	for (i = 0; i != t->extra_c; i++) {
		out->extras[i].make_extra = strdup(t->extras[i].make_extra);
		out->extras[i].select = t->extras[i].select;
	}
	
	return out;
}

void host_template_list_init (Server* srv) {
	for (int i = 0; i != sizeof(host_templates_init) / sizeof(HostTemplate); i++)
		host_template_add(srv, &host_templates_init[i]);
}

void host_template_add (Server* srv, HostTemplate* ht) {
	HostTemplate* temp = prv_host_template_alloc(ht);
	vector_add(srv->templates, &temp);
}

/*
StringVector* host_template_get_all() {
	StringVector* out = string_vector_new();
	int i;
	for (i = 0; i != sizeof(host_templates) / sizeof(host_templates[0]); i++) {
		string_vector_add(out, host_templates[i].id);
	}
	
	return out;
}*/

HostTemplate* stage_new (Server* parent, int index) {
	if (!parent->templates || index >= parent->templates->n)
		return NULL;
	
	return host_template_init(parent, *vector_get(parent->templates, index));
}

HostTemplate* host_template_init (Server* parent, HostTemplate* t) {
	HostTemplate* out = prv_host_template_alloc(t);
	out->parent = parent;
	
	out->new_id = host_id_new();
	asprintf(&out->dest_dir, "%s/stage-%s", parent->location, out->new_id);
	
	return out;
}

char* host_template_download (HostTemplate* t) {
	char distfile_dir[256];
	sprintf(distfile_dir, "%s/distfiles", t->parent->location);
	
	if (!opendir(distfile_dir)) {
		mkdir(t->dest_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}
	
	char distfile_meta_url[256];
	sprintf(distfile_meta_url,
			"http://distfiles.gentoo.org/releases/%s/autobuilds/latest-stage3-%s.txt",
			t->arch,
			t->id);
	
	int dl_ret;
	command("wget", "download to quiet", NULL, &dl_ret, distfile_meta_url, "temp_dest");
	
	
	if (dl_ret != 0) {
		lerror("Could not download metadata for stage3!");
		return NULL;
	}
	
	FILE* fp_temp = fopen("temp_dest", "r");
	
	char* line;
	size_t len;
	char* stage3_dest = NULL;
	ssize_t read = 0;
	char* stage3_date = NULL;
	while ((read = getline(&line, &len, fp_temp)) != -1) {
		if (!line || line[0] == '#')
			continue;
		// The first non-comment line will be our target stage3
		
		line[strlen(line) - 1] = '\0'; // Remove the newline
		if (strlen(line) == 0)
			continue;
		
		char* dup_line = strdup(line);
		char* s = strtok(line, "/");
		stage3_date = strdup(s);
		s = strtok(NULL, " ");
		asprintf(&stage3_dest, "http://distfiles.gentoo.org/releases/%s/autobuilds/%s/%s", t->arch, stage3_date, s);
		free(dup_line);
		free(stage3_date);
		
		break;
	}
	if (stage3_dest == NULL)
		return NULL;
	fclose(fp_temp);
	remove("temp_dest");
	
	char* fname;
	asprintf(&fname, "%s/distfiles/stage3-%s-%s.tar.bz2", t->parent->location, t->id, stage3_date);
	
	prv_mkdir("distfiles");
	
	if (access(fname, F_OK) == -1) {
		linfo("Downloading stage3 from %s", stage3_dest);
		command("wget", "download to", NULL, &dl_ret, stage3_dest, fname);
	}
	free(stage3_dest);
	
	return fname;
}

response_t host_template_extract (HostTemplate* t, char* fname) {
	int ext_ret;
	prv_mkdir(t->dest_dir);
	command("tar", "extract to", NULL, &ext_ret, fname, t->dest_dir);
	
	if (ext_ret != 0) {
		lerror("Failed to extract stage3 tar");
		return INTERNAL_ERROR;
	}
	
	return OK;
}

response_t host_template_stage (HostTemplate* t) {
	char* fname = host_template_download(t);
	if (!fname)
		return INTERNAL_ERROR;
	
	return host_template_extract(t, fname);
}

Host* host_template_handoff (HostTemplate* src) {
	Host* out = host_new(src->parent, strdup(src->new_id));
	char old_stage[32];
	sprintf(old_stage, "stage-%s", src->new_id);
	
	out->extra = string_vector_new();
	if (rename(old_stage, out->id) != 0) {
		lerror("Failed to rename %s to %s", old_stage, out->id);
		free(out->id);
		free(out);
		string_vector_free(out->extra);
		return NULL;
	}
	
	out->hostname = strdup("default");
	out->use = strdup(""); // Give it an empty just in case
	
	// Profile
	{
		char* t_profile_l;
		char profile_dest[256];
		
		asprintf(&t_profile_l, "%s/etc/portage/make.profile", out->id);
		ssize_t profile_len = readlink(t_profile_l, profile_dest, sizeof(profile_dest) - 1);
		profile_dest[(int) profile_len] = 0; // Readlink does not null terminate
		
		char* t_profile_split = strstr(profile_dest, "profiles/");
		if (t_profile_split)
			out->profile = strdup(t_profile_split + strlen("profiles/"));
		free(t_profile_l);
	}
	
	// defaults
	{
		string_overwrite(&out->cxxflags, "${CFLAGS}", 1);
		string_overwrite(&out->portage_tmpdir, "/autogentoo/tmp", 1);
		string_overwrite(&out->portdir, "/usr/portage", 1);
		string_overwrite(&out->distdir, "/usr/portage/distfiles", 1);
		string_overwrite(&out->pkgdir, "/autogentoo/pkg", 1);
		string_overwrite(&out->port_logdir, "/autogentoo/log", 1);
	}
	
	// make.conf stuff
	{
		out->cflags = strdup(src->cflags);
		out->arch = strdup(src->arch);
		out->chost = strdup(src->chost);
	}
	
	char** _t[] = {
			NULL,
			&out->cxxflags,
			&out->portage_tmpdir,
			&out->portdir,
			&out->distdir,
			&out->pkgdir,
			&out->port_logdir
	};
	
	size_t _t_size = sizeof(_t) / sizeof(_t[0]);
	
	int i;
	for (i = 0; i != src->extra_c; i++) {
		if (src->extras[i].select == OTHER) {
			string_vector_add(out->extra, src->extras[i].make_extra);
			continue;
		}
		
		int j = 0;
		for (int temp = src->extras[i].select; temp >>= 1; j++);
		if (j < _t_size && j > 0)
			*_t[j] = strdup(src->extras[i].make_extra);
	}
	
	src = small_map_delete_index(src->parent->stages,);
	host_template_free(src);
	return out;
}


void host_template_free (HostTemplate* temp) {
	free(temp->id);
	free(temp->arch);
	free(temp->cflags);
	free(temp->chost);
	
	int i;
	for (i = 0; i != temp->extra_c; i++) {
		free(temp->extras->make_extra);
	}
	
	free(temp->dest_dir);
	free(temp->new_id);
	free(temp);
}