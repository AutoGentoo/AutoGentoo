from op_socket cimport Address
from vector cimport PyVec
from d_malloc cimport Binary

cdef extern from "<autogentoo/writeconfig.h>":
	cdef enum:
		AUTOGENTOO_FILE_END = 0xffffffff,
		AUTOGENTOO_HOST = 0xfffffff0,
		AUTOGENTOO_HOST_BINDING = 0xffffff00,
		AUTOGENTOO_STAGE = 0xfffff000,
		AUTOGENTOO_HOST_END = 0xaaaaaaaa,
		AUTOGENTOO_HOST_KERNEL = 0xbbbbbbbb
	
	ctypedef enum chroot_t:
		CHR_NOT_MOUNTED,
		CHR_MOUNTED
	
	ctypedef enum template_selects:
		OTHER = 0x00,
		CXXFLAGS = 0x01,
		TMPDIR = 0x02,
		PORTDIR = 0x04,
		DISTDIR = 0x08,
		PKGDIR = 0x10,
		LOGDIR = 0x20
	

cdef class Server:
	cdef Address adr
	cdef readonly char* target;
	
	cdef readonly hosts
	cdef readonly stages
	cdef readonly templates
	cdef sock
	
	cpdef void read_server (self)

cdef class Host:
	cdef readonly Server parent # The parent server
	cdef readonly char* id # The ID of the Host
	cdef readonly char* profile # Portage profile, see possible values with eselect profile list
	cdef readonly char* hostname # Hostname of the host (shows up in the graphical client)
	
	cdef readonly char* cflags # The gcc passed to C programs, try -march=native :)
	cdef readonly char* cxxflags # The gcc passed only to CXX programs
	cdef readonly char* arch #  The portage-arch (eg. amd64)
	cdef readonly char* chost # The system chost (should not be changed after it is set)
	cdef readonly char* use # use flags
	cdef readonly PyVec extra # A list of extra entries to go into make.conf
	
	cdef readonly char* portage_tmpdir # build dir
	cdef readonly char* portdir # ebuild portage tree
	cdef readonly char* distdir # distfiles
	cdef readonly char* pkgdir # path to binaries
	cdef readonly char* port_logdir # logs
	cdef readonly chroot_t chroot_status
	cdef readonly PyVec kernel
	
	cdef void parse (self, Binary _bin)

cdef struct StageExtra:
	char* make_extra
	template_selects select

cdef StageExtra* stage_extra (char* m_e, template_selects sel)

cdef class Stage:
	cdef readonly char* id
	cdef readonly char* arch
	cdef readonly char* cflags
	cdef readonly char* chost
	
	cdef readonly PyVec extras
	
	cdef readonly char* dest_dir
	cdef readonly char* parent
	cdef readonly char* new_id
	
	cdef parse (self, Binary _bin)