from d_malloc cimport Binary
from interface cimport Server

cdef enum JobType:
	HANDOFF = 1,
	MAKECONF

cdef class Job:
	cdef Server srv
	cdef Binary message
	
	cdef JobType type
	cdef char* template
	cdef objects

cpdef job_handoff (stage_id, hostname, profile)
cpdef job_makeconf ()

# List of jobs
cdef job_link = {
	HANDOFF: job_handoff,
	MAKECONF: job_makeconf,
}