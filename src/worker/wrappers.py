class AccessLevel:
	NO_PERM = 0
	READ = 1 << 0
	WRITE = 1 << 1
	EXEC = 1 << 2


class ReturnT:
	SCRIPT_NOT_FOUND = -1
	SCRIPT_OK = 0
	SCRIPT_FILE_NOT_FOUND = 1
	SCRIPT_PERMISSION_DENIED = 1


class WorkerScripts:
	def __init__(self):
		self.script_links = {}
	
	def authenticate(self, ):
	
	def script(self, function, auth_level:int=0):
		def wrapper(instance, **kwargs):
			
			return function(instance, **kwargs)
		
		self.script_links[function.__name__] = wrapper
		return wrapper
	
	def run(self, name, **kwargs):
		try:
			self.script_links[name]
		except KeyError:
			return ReturnT.SCRIPT_NOT_FOUND, None
		
		self.script_links[name]()
