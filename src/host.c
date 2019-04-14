#define _GNU_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <autogentoo/host.h>
#include <string.h>
#include <limits.h>
#include <sys/wait.h>
#include <autogentoo/user.h>
#include <errno.h>
#include <autogentoo/writeconfig.h>

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
	out->environment_status = HOST_ENV_VOID;
	
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

char* host_path(Host* host, char* sub) {
	char buf[PATH_MAX];
	char* dest_temp = realpath(host->parent->location, buf);
	if (dest_temp == NULL) {
		lerror("Failed to get realpath(%s)", host->parent->location);
		lerror("Error [%d] %s", errno, strerror(errno));
		return NULL;
	}
	
	char* out;
	asprintf(&out, "%s/%s/%s", dest_temp, host->id, sub);
	return out;
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

int host_write_make_conf(Host* host) {
	return 0;
}

int read_int(FILE* fp);
size_t write_int(int src, FILE* fp);

void host_getstatus(Host* host) {
	char* host_directory = host_path(host, "");
	struct stat st;
	
	if (stat(host_directory, &st) != 0) {
		lerror("Failed to open %s", host_directory);
		lerror("Error [%d] %s", errno, strerror(errno));
		host->environment_status = HOST_ENV_VOID;
		write_server(host->parent);
		free(host_directory);
		
		return;
	}
	
	if (!S_ISDIR(st.st_mode)) {
		lerror("Environment root %s is not a directory", host_directory);
		host->environment_status = HOST_ENV_VOID;
		write_server(host->parent);
		free(host_directory);
		
		return;
	}
	
	char* fp_stat_fn;
	asprintf(&fp_stat_fn, "%s/.env_status", host_directory);
	free(host_directory);
	
	FILE* fp_stat = fopen(fp_stat_fn, "rb");
	free(fp_stat_fn);
	if (!fp_stat) {
		host->environment_status = HOST_ENV_VOID;
		write_server(host->parent);
		return;
	}
	
	host->environment_status = read_int(fp_stat);
	fclose(fp_stat);
	
	write_server(host->parent);
}

int host_setstatus(Host* host) {
	char* host_directory = host_path(host, "");
	if (!host_directory)
		return 1;
	
	struct stat st;
	
	if (stat(host_directory, &st) != 0) {
		lerror("Error on stat [%d] %s", errno, strerror(errno));
		
		host->environment_status = HOST_ENV_VOID;
		
		if (errno == ENOENT) {
			linfo("Creating environment directory %s", host_directory);
			int status = mkdir(host_directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			
			if (status != 0) {
				lerror("Error creating directory");
				lerror("Error [%d] %s", errno, strerror(errno));
				
				free(host_directory);
				return status;
			}
		}
	}
	
	char* fp_stat_fn;
	asprintf(&fp_stat_fn, "%s/.env_status", host_directory);
	free(host_directory);
	
	FILE* fp_stat = fopen(fp_stat_fn, "w+");
	free(fp_stat_fn);
	if (!fp_stat)
		return -1;
	
	write_int(host->environment_status, fp_stat);
	fclose(fp_stat);
	
	return 0;
}