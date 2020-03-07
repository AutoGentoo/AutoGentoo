import sys


class Log:
	output_file: str
	print_stdout: bool
	fp = None
	
	ANSI_RED = "\x1b[31m"
	ANSI_GREEN = "\x1b[32m"
	ANSI_YELLOW = "\x1b[33m"
	ANSI_BLUE = "\x1b[34m"
	ANSI_MAGENTA = "\x1b[35m"
	ANSI_CYAN = "\x1b[36m"
	ANSI_RESET = "\x1b[0m"
	ANSI_BOLD = "\x1b[1m"
	ANSI_COLOR = "\x1b[39m"
	
	def __init__(self, target: str, print_stdout=True):
		self.output_file = target
		self.print_stdout = print_stdout
		
		self.fp = open(self.output_file, "a+")
	
	def __log_gen__(self, message: str, star_color: str) -> str:
		return "%s %s * %s %s %s\n" % (self.ANSI_BOLD, star_color, self.ANSI_COLOR, message, self.ANSI_RESET)
	
	def __log__(self, message, star_color):
		out_str = self.__log_gen__(message, star_color)
		if self.print_stdout:
			sys.stdout.write(out_str)
		
		self.fp.write(out_str)
	
	def info(self, message):
		self.__log__(message, self.ANSI_GREEN)
	
	def warn(self, message):
		self.__log__(message, self.ANSI_YELLOW)
	
	def error(self, message, code=-1):
		self.__log__("[%d] %s" % (code, message) if code != -1 else message, self.ANSI_RED)
