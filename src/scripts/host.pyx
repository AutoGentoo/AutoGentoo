from interface import Server, Host

cdef extern from "<autogentoo/host.h>":
	ctypedef enum chroot_t:
		CHR_NOT_MOUNTED,
		CHR_MOUNTED
	
	cdef struct __Host:
		void* parent
		char* id
		char* profile
		char* hostname
		chroot_t chroot_status
		
		char* cflags
		char* cxxflags;
		char* arch;
		char* chost;
		char* use;
		char[:]* extra;
		char* portage_tmpdir;
		char* portdir;
		char* distdir;
		char* pkgdir;
		char* port_logdir;
		void* kernel;

cdef extern from "<autogentoo/write_config.h>":
	ctypedef enum AutoGentoo_WriteConfig:
		AUTOGENTOO_FILE_END = 0xffffffff,
		AUTOGENTOO_HOST = 0xfffffff0,
		AUTOGENTOO_HOST_BINDING = 0xffffff00,
		AUTOGENTOO_STAGE = 0xfffff000,
		
		# Because Host* is extensible this is required
		AUTOGENTOO_HOST_END = 0xaaaaaaaa,
		
		# Host entries for autogentoo extensions
		AUTOGENTOO_HOST_KERNEL = 0xbbbbbbbb

