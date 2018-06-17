from d_malloc cimport Binary
from interface cimport Server

cdef extern from "<autogentoo/queue.h>":
	ctypedef enum queue_t:
		QUEUE_END,
		QUEUE_HANDOFF,
		QUEUE_MAKECONF


cdef class Job:
	cdef Server srv
	cdef Binary message
	
	cdef queue_t type
	cdef char* template
	cdef objects

cpdef job_handoff (Server srv, int stage_id, char* hostname, char* profile)
cpdef job_makeconf (Server srv, char* hostid)

# List of jobs
cdef job_link = {
	QUEUE_HANDOFF: job_handoff,
	QUEUE_MAKECONF: job_makeconf,
}