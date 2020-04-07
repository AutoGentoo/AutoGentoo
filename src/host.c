#define _GNU_SOURCE

#include <stdlib.h>
#include <autogentoo/host.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <autogentoo/writeconfig.h>
#include <sys/stat.h>
#include <autogentoo/host_environment.h>

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
	return prv_gen_random(AUTOGENTOO_HOST_ID_LENGTH);
}

Host* host_new(Server* server, char* id) {
	Host* out = malloc(sizeof(Host));
	out->parent = server;
	out->id = id; // Dont need to dup, never accessed elsewhere
	
	pthread_mutex_init(&out->cs_mutex, NULL);
	out->__chroot_status__ = CHR_NOT_MOUNTED;
	out->environment_status = HOST_ENV_VOID;
	out->environment = malloc(sizeof(HostEnvironment));
	
	out->arch = NULL;
	out->hostname = NULL;
	out->profile = NULL;
	
	out->environment->cflags = strdup("-O2 -pipe");
	out->environment->cxxflags = strdup("${CFLAGS}");
	out->environment->use = strdup("");
	out->environment->portage_tmpdir = strdup("/autogentoo/tmp");
	out->environment->portdir = strdup("/usr/portage");
	out->environment->distdir = strdup("/usr/portage/distfiles");
	out->environment->pkgdir = strdup("/autogentoo/pkg");
	out->environment->portage_logdir = strdup("/autogentoo/log");
	out->environment->lc_messages = strdup("C");
	
	out->environment->extra = small_map_new(5);
	
	out->kernel = NULL;
	
	host_init_extras(out);
	
	return out;
}

void host_init_extras(Host* target) {
	target->kernel = vector_new(VECTOR_UNORDERED | VECTOR_REMOVE);
}

char* host_path(Host* host, char* sub, ...) {
	char buf[PATH_MAX];
	char* dest_temp = realpath(host->parent->location, buf);
	if (dest_temp == NULL) {
		lerror("Failed to get realpath(%s)", host->parent->location);
		lerror("Error [%d] %s", errno, strerror(errno));
		return NULL;
	}
	
	char* out = malloc(256);
	out[0] = 0;
	
	sprintf(out, "%s/%s/", dest_temp, host->id);
	
	va_list args;
	va_start(args, sub);
	
	vsprintf(out + strlen(out), sub, args);
	
	va_end(args);
	
	return out;
}

void host_free(Host* host) {
	free(host->id);
	free(host->hostname);
	free(host->profile);
	free(host->arch);
	
	pthread_mutex_destroy(&host->cs_mutex);
	host_environment_free(host->environment);
	
	if (host->kernel != NULL) {
		int i;
		for (i = 0; i != host->kernel->n; i++)
			kernel_free((Kernel*) vector_get(host->kernel, i));
		vector_free(host->kernel);
	}
	
	free(host);
}

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
		mkdir(host_path(host, ""), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		lwarning("Recreating host directory %s", host->id);
		
		host->environment_status = HOST_ENV_VOID;
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

int host_init(Host* host) {
	char* host_directory = host_path(host, "");
	if (!host_directory)
		return 1;
	
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
	
	return 0;
}

chroot_t host_get_chroot(Host* h) {
	pthread_mutex_lock(&h->cs_mutex);
	chroot_t out = h->__chroot_status__;
	pthread_mutex_unlock(&h->cs_mutex);
	
	return out;
}

void host_set_chroot(Host* h, chroot_t target) {
	pthread_mutex_lock(&h->cs_mutex);
	h->__chroot_status__ = target;
	pthread_mutex_unlock(&h->cs_mutex);
}