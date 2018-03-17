from libc.stdio cimport *
import sys

cdef char* ANSI_RED = b"\x1b[31m"
cdef char* ANSI_GREEN = b"\x1b[32m"
cdef char* ANSI_YELLOW = b"\x1b[33m"
cdef char* ANSI_BLUE = b"\x1b[34m"
cdef char* ANSI_MAGENTA = b"\x1b[35m"
cdef char* ANSI_CYAN = b"\x1b[36m"
cdef char* ANSI_RESET = b"\x1b[0m"
cdef char* ANSI_BOLD = b"\x1b[1m"
cdef char* ANSI_COLOR = b"\x1b[39m"

cdef class Log:
	kw_handlers = {
		"newline": lambda: printf ("\n"),
		"flush": lambda: fflush (stdout)
	}
	
	@staticmethod
	def common (kwargs):
		fputs (ANSI_RESET, stdout)
		
		for key in Log.kw_handlers:
			try:
				kwargs[key]
			except KeyError:
				pass
			else:
				if kwargs[key]:
					Log.kw_handlers[key]()
	
	@staticmethod
	def info (text, *va_args, **kwargs):
		try:
			kwargs["color_only"]
		except KeyError:
			printf ("%s%s * %s", ANSI_BOLD, ANSI_GREEN, ANSI_COLOR)
		else:
			if kwargs["color_only"]:
				printf ("%s%s", ANSI_BOLD, ANSI_COLOR)
			else:
				printf ("%s%s * %s", ANSI_BOLD, ANSI_GREEN, ANSI_COLOR)
		
		fputs((text % va_args).encode ("UTF-8"), stdout)
		Log.common(kwargs)
		
	
	@staticmethod
	def error (text, *va_args, **kwargs):
		printf (b"%s%s * %s", ANSI_BOLD, ANSI_RED, ANSI_COLOR)
		fflush (stdout)
		fputs((text % va_args).encode ("UTF-8"), stdout)
		Log.common(kwargs)
	
	@staticmethod
	def warn (text, *va_args, **kwargs):
		printf ("%s%s * %s", ANSI_BOLD, ANSI_YELLOW, ANSI_COLOR)
		fputs((text % va_args).encode ("UTF-8"), stdout)
		Log.common(kwargs)