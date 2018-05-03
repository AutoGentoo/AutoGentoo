from d_malloc cimport DynamicBuffer

cdef extern from "<autogentoo/request.h>":
	ctypedef enum protocol_t:
		PROT_AUTOGENTOO = 0, # guarentees first byte is 0 (cant be HTTP)
		PROT_HTTP
	
	ctypedef enum request_t:
		REQ_GET,
		REQ_INSTALL,
		REQ_EDIT,
		REQ_ACTIVATE,
		REQ_HOSTREMOVE,
		REQ_MNTCHROOT,
		REQ_GETHOSTS,
		REQ_GETHOST,
		REQ_GETACTIVE,
		REQ_GETSPEC,
		REQ_GETTEMPLATES,
		REQ_STAGE_NEW,
		REQ_TEMPLATE_CREATE,
		REQ_STAGE,
		REQ_GETSTAGED,
		REQ_GETSTAGE,
		REQ_HANDOFF,
		REQ_SAVE,
		REQ_HOSTWRITE,
		REQ_HOSTUPLOAD,
		
		# Binary requests
		REQ_BINSERVER,
		
		# General
		REQ_EXIT
	
	cdef packed struct __HostEdit:
		int selection_one;
		int selection_two; # !< -1 for none, >= 0 for vector access at offset_1
		
		char* edit;
	
	cdef packed struct __MakeExtraPack:
		char* make_extra
		int select
	
	cdef packed struct __TemplateCreate:
		char* id;
		char* arch;
		char* cflags;
		char* chost;
		int make_extra_c;
		__MakeExtraPack* extras
	
	cdef packed struct __HostSelect:
		char* host_id
	
	cdef packed struct __StageSelect:
		int index;
	
	cdef packed struct __HostInstall:
		char* argument;
	
	ctypedef enum stage_command_t:
		STAGE_NONE = 0,
		STAGE_DOWNLOAD = 0x1,
		STAGE_EXTRACT = 0x2,
		STAGE_ALL = STAGE_DOWNLOAD | STAGE_EXTRACT
	
	cdef packed struct __StageCommand:
		stage_command_t command;
	
	cdef packed struct __HostOffset:
		size_t offset;
		size_t size;
		void* ptr;
	
	ctypedef enum request_structure_t:
		STRCT_END,
		STRCT_HOSTEDIT,
		STRCT_HOSTSELECT,
		STRCT_HOSTINSTALL,
		STRCT_TEMPLATECREATE,
		STRCT_TEMPLATESELECT,
		STRCT_STAGECOMMAND,
		STRCT_HOSTOFFSET, # /* Custom offset on Host struct */
	
cdef union RequestData:
	__HostEdit he
	__HostSelect hs;
	__HostInstall hi;
	__TemplateCreate tc;
	__StageSelect ss;
	__StageCommand sc;
	__HostOffset ho;

cdef extern from "<autogentoo/host.h>":
	ctypedef enum hostoffsets_t:
		HOSTOFF_ID,
		HOSTOFF_PROFILE,
		HOSTOFF_HOSTNAME,
		HOSTOFF_CHROOT_STATUS,
		HOSTOFF_CFLAGS,
		HOSTOFF_CXXFLAGS,
		HOSTOFF_ARCH,
		HOSTOFF_CHOST,
		HOSTOFF_USE,
		HOSTOFF_EXTRA,
		HOSTOFF_PORTAGE_TMPDIR,
		HOSTOFF_PORTDIR,
		HOSTOFF_DISTDIR,
		HOSTOFF_PKGDIR,
		HOSTOFF_PORT_LOGDIR

host_offset_binding = {
	"ID": HOSTOFF_ID,
	"PROFILE": HOSTOFF_PROFILE,
	"HOSTNAME": HOSTOFF_HOSTNAME,
	"CHROOT_STATUS": HOSTOFF_CHROOT_STATUS,
	"CFLAGS": HOSTOFF_CFLAGS,
	"CXXFLAGS": HOSTOFF_CXXFLAGS,
	"ARCH": HOSTOFF_ARCH,
	"CHOST": HOSTOFF_CHOST,
	"USE": HOSTOFF_USE,
	"EXTRA": HOSTOFF_EXTRA,
	"PORTAGE_TMPDIR": HOSTOFF_PORTAGE_TMPDIR,
	"PORTDIR": HOSTOFF_PORTDIR,
	"DISTDIR": HOSTOFF_DISTDIR,
	"PKGDIR": HOSTOFF_PKGDIR,
	"PORT_LOGDIR": HOSTOFF_PORT_LOGDIR
}

from collections import namedtuple
MakeExtra = namedtuple('MakeExtra', 'make_extra select')

cdef class Request:
	cdef DynamicBuffer data
	cdef request_t type
	cdef protocol_t protocol
	cdef int* count_ptr
	
	cdef add_hostedit (self, int select_one, int select_two, char* field)
	cdef add_hostselect (self, char* host_id)
	cdef add_hostinstall (self, char* param)
	cdef add_templatecreate (self, char* host_id, char* arch, char* cflags, char* chost, extras=*)
	cdef add_templateselect (self, int index)
	cdef add_stagecommand (self, stage_command_t command)
	cdef add_hostupload_int (self, hostoffsets_t offset, int ob)
	cdef add_hostupload_str (self, hostoffsets_t offset, char* ob)
	cdef add_hostupload_list (self, hostoffsets_t offset, list ob)
	cdef add_int (self, int to_add)
	cdef finish (self)