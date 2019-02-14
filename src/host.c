#define _GNU_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <autogentoo/host.h>
#include <string.h>
#include <limits.h>
#include <sys/wait.h>
#include <autogentoo/user.h>

char* prv_gen_random(size_t len) {
	char* out = malloc((size_t) len + 1);
	
	srandom((unsigned int) time(NULL));  // Correct seeding function for random()
	
	int i;
	for (i = 0; i != len; i++) {
		char c;
		c = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"[random() % 62];
		out[i] = c;
	}
	out[len] = 0;
	
	return out;
}

inline char* host_id_new() {
	return prv_gen_random(AUTOGETNOO_HOST_ID_LENGTH);
}

Host* host_new(Server* server, char* id) {
	Host* out = malloc(sizeof(Host));
	out->parent = server;
	out->id = id; // Dont need to dup, never accessed elsewhere
	out->chroot_status = CHR_NOT_MOUNTED;
	
	out->arch = NULL;
	out->hostname = NULL;
	out->profile = NULL;
	out->make_conf = small_map_new(20, 5);
	
	small_map_insert(out->make_conf, "CFLAGS", strdup("-O2 -pipe"));
	small_map_insert(out->make_conf, "CXXFLAGS", strdup("${CFLAGS}"));
	small_map_insert(out->make_conf, "USE", strdup(""));
	small_map_insert(out->make_conf, "PORTAGE_TMPDIR", strdup("/autogentoo/tmp"));
	small_map_insert(out->make_conf, "PORTDIR", strdup("/usr/portage"));
	small_map_insert(out->make_conf, "DISTDIR", strdup("/usr/portage/distfiles"));
	small_map_insert(out->make_conf, "PKGDIR", strdup("/autogentoo/pkg"));
	small_map_insert(out->make_conf, "PORT_LOGDIR", strdup("/autogentoo/log"));
	
	out->kernel = NULL;
	
	host_init_extras(out);
	
	return out;
}

void host_init_extras(Host* target) {
	target->kernel = vector_new(sizeof(Kernel*), VECTOR_UNORDERED | VECTOR_REMOVE);
}

void host_get_path (Host* host, char** dest) {
	char buf[PATH_MAX];
	char* dest_temp = realpath(host->parent->location, buf);
	if (dest_temp == NULL) {
		lerror("Failed to get realpath()");
		return;
	}
	
	asprintf(dest, "%s/%s", dest_temp, host->id);
}

void host_free(Host* host) {
	free(host->id);
	free(host->hostname);
	free(host->profile);
	free(host->arch);
	
	if (host->kernel != NULL) {
		int i;
		for (i = 0; i != host->kernel->n; i++)
			kernel_free(*(Kernel**) vector_get(host->kernel, i));
		vector_free(host->kernel);
	}
	
	free(host);
}

/*
arch_t determine_arch (char* chost) {
    char* dest = strdup (chost);
    char* to_clear = strchr (dest, '-');
    if (to_clear == NULL) {
        return _INVALIDBIT;
    }
    *to_clear = 0;
    
    char* supported_32bit[] = {
        "i386",
        "i486",
        "i586",
        "i686"
    };
    
    char* supported_64bit[] = {
        "x86_64"
    };
    
    if (string_find(supported_32bit, dest, sizeof(supported_32bit) / sizeof(*supported_32bit)) != -1) {
        free (dest);
        return _32BIT;
    }
    
    if (string_find(supported_64bit, dest, sizeof(supported_64bit) / sizeof(*supported_64bit)) != -1) {
        free (dest);
        return _64BIT;
    }
    
    free (dest);
    return _INVALIDBIT;
}
*/

int host_write_make_conf(Host* host) {
	return 0;
}

/*
 * Deprecated

response_t host_init (Host* host) {
    char host_path[256];
    host_path[0] = 0;
    host_get_path(host, host_path);
    
    if (host_path[0] == 0) {
        return INTERNAL_ERROR;
    }
    
    linfo ("Initializing host in %s", host_path);
    
    char *new_dirs [] = {
        host->portage_tmpdir,
        host->pkgdir,
        host->port_logdir,
        host->portdir,
        "etc/portage",
        "usr",
        "lib32",
        "lib64",
        "usr/lib32",
        "usr/lib64",
        "proc",
        "sys",
        "dev"
    };
    
    char target_dir[256];
    
    int i;
    for (i=0; i!=sizeof (new_dirs) / sizeof (new_dirs[0]); i++) {
        target_dir[0] = 0;
        sprintf (target_dir, "%s/%s", host_path, new_dirs[i]);
        prv_mkdir(target_dir);
    }
    
    linfo ("Writing make.conf");
    if (host_write_make_conf (host)) {
        lerror ("Failed writing make.conf!");
        return INTERNAL_ERROR;
    }
    
    arch_t current_arch = determine_arch (host->chost);
    if (current_arch == _INVALIDBIT) {
        lerror ("invalid/unsupported chost: %s", host->chost);
        return INTERNAL_ERROR;
    }
    
    linfo ("Linking directories/profile");
    pid_t link_fork = fork();
    if (link_fork == -1) {
        exit (-1);
    }
    if (link_fork == 0) {
        chdir (host_path);
        
        int ret = 0;
        
        // Create the profile symlink
        char sym_buf_p1 [128];
        sprintf (sym_buf_p1, "/usr/portage/profiles/%s/", host->profile);
        
        char* lib_dest = current_arch == _32BIT ? "lib32" : "lib64";
        
        char* links[][2] = {
            {sym_buf_p1, "etc/portage/make.profile"},
            {lib_dest, "lib"},
            {lib_dest, "usr/lib"}
        };
        
        // Check if symlinks exist and remove them
        struct stat __sym_buff;
        
        int i;
        for (i = 0; i != sizeof (links) / sizeof (*links); i++) {
            if (lstat (links[i][1], &__sym_buff) == 0) {
                unlink (links[i][1]);
            }
            
            linfo ("Linking %s to %s", links[i][0], links[i][1]);
            if (symlink (links[i][0], links[i][1]) != 0) {
                lwarning("Failed to symlink %s!", links[i][1]);
                lwarning ("%d", errno);
                ret = 1;
            }
        }
        
        exit(ret);
    }
    
    int link_return;
    waitpid (link_fork, &link_return, 0); // Wait until finished
    
    host->status = INIT;
    
    return link_return ? INTERNAL_ERROR : OK;
}
*/

/*
response_t host_stage1_install (Host* host, char* arg) {
    String* cmd_full = string_new (128);
    string_append(cmd_full, "emerge --autounmask-continue --buildpkg --root=\'");
    string_append(cmd_full, host->parent->location);
    string_append_c(cmd_full, '/');
    string_append(cmd_full, host->id);
    string_append(cmd_full, "\' --config-root=\'");
    string_append(cmd_full, host->parent->location);
    string_append_c(cmd_full, '/');
    string_append(cmd_full, host->id);
    string_append(cmd_full, "\' ");
    string_append(cmd_full, arg);
    
    printf ("%s\n", cmd_full->ptr);
    
    char* args[64];
    int i;
    for (i = 0, args[i] = strtok(cmd_full->ptr, " "); args[i] != NULL; i++, args[i] = strtok (NULL, " "));
    
    pid_t install_pid = fork ();
    if (install_pid == 0) {
        linfo ("Starting emerge...");
        fflush (stdout);
        
        execv ("/usr/bin/emerge", args);
        exit(-1);
    }
    
    int install_ret;
    waitpid (install_pid, &install_ret, 0); // Wait until finished
    
    string_free (cmd_full);
    
    return install_ret == 0 ? OK : INTERNAL_ERROR;
}*/

response_t host_install(Host* host, char* arg) {
	
	char* new_line;
	if ((new_line = strchr(arg, '\n')) != NULL) {
		*new_line = 0;
	}
	
	char* temp;
	asprintf(&temp, "emerge --autounmask-continue --buildpkg %s", arg);
	
	char* k;
	int spaces = 0;
	for (k = temp; *k != 0; k++)
		if (*k == ' ')
			spaces++;
	
	char** args = malloc(sizeof(char*) * (spaces + 2));
	char** pos;
	for (k = strtok(temp, " "), pos = args; k != NULL; pos++, k = strtok(NULL, " "))
		*pos = k;
	*pos = NULL;
	
	pid_t install_pid = fork();
	if (install_pid == 0) {
		char* root;
		host_get_path(host, &root);
		if (chdir(root) == -1) {
			lerror("chdir() failed");
			exit(-1);
		}
		if (chroot(root) == -1) {
			lerror("chroot() failed");
			exit(-1);
		}
		free (root);
		
		linfo("Starting emerge...");
		fflush(stdout);
		
		execv("/usr/bin/emerge", args);
		exit(-1);
	}
	
	int install_ret;
	waitpid(install_pid, &install_ret, 0); // Wait until finished
	free(temp);
	free(args);
	
	return install_ret == 0 ? OK : INTERNAL_ERROR;
	
}