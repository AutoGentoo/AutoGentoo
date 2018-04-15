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
	
	ctypedef enum request_structure_t:
		STRCT_HOSTEDIT = 1,
		STRCT_HOSTSELECT,
		STRCT_HOSTINSTALL,
		STRCT_TEMPLATECREATE,
		STRCT_STAGESELECT,
		STRCT_STAGECOMMAND
	
cdef union RequestData:
	__HostEdit he
	__HostSelect hs;
	__HostInstall hi;
	__TemplateCreate tc;
	__StageSelect ss;
	__StageCommand sc;

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
	cdef add_stageselect (self, int index)
	cdef add_stagecommand (self, stage_command_t command)
	cdef increment (self)
	cdef add_int (self, int to_add)