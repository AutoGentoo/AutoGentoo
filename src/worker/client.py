from autogentoo_api.d_malloc import BinaryFileReader

AUTOGENTOO_ACCESS_TOKEN = 0xdddddddd
AUTOGENTOO_HOST = 0xfffffff0
AUTOGENTOO_SERVER_TOKEN = 0xfffff000
AUTOGENTOO_HOST_END = 0xaaaaaaaa
AUTOGENTOO_FILE_END = 0xffffffff
AUTOGENTOO_SUDO_TOKEN = 0xcccccccc


class Server:
	def __init__(self, path):
		self.path = path
		self.file = "%s/.autogentoo.config" % self.path
		
		self.reader = None
		
		self.hosts = []
		self.keys = {}
		
		self.sudo_token = None
		self.autogentoo_org_token = None
	
	def read_host(self):
		host = Host(self.reader, self)
		host.read()
		self.hosts.append(host)
	
	def read_token(self):
		token = Token(self.reader)
		token.read()
		self.keys[token.auth_token] = token
	
	def read(self):
		self.reader = BinaryFileReader(self.file)
		self.keys = {}
		self.hosts = []
		
		self.sudo_token = None
		self.autogentoo_org_token = None
		
		current = self.reader.read_int() & 0xffffffff
		while current != AUTOGENTOO_FILE_END:
			if current == AUTOGENTOO_HOST:
				self.read_host()
			elif current == AUTOGENTOO_SERVER_TOKEN:
				self.autogentoo_org_token = self.reader.read_string()
			elif current == AUTOGENTOO_SUDO_TOKEN:
				self.sudo_token = self.reader.read_string()
			elif current == AUTOGENTOO_ACCESS_TOKEN:
				self.read_token()
			else:
				raise IOError("Invalid data type %s" % hex(current))
			
			current = self.reader.read_int() & 0xffffffff
		self.reader = None
	
	def get_host(self, host_id):
		if host_id is None:
			return None
		
		for host in self.hosts:
			if host.id == host_id:
				return host
		return None


class Host:
	def __init__(self, reader, parent):
		self.reader = reader
		self.parent = parent
		
		self.id = None
		self.status = -1
		self.env_status = -1
		
		self.hostname = ""
		self.profile = ""
		self.arch = ""
		
		self.cflags = ""
		self.cxxflags = ""
		self.distdir = ""
		self.lc_messages = ""
		self.pkgdir = ""
		self.portage_logdir = ""
		self.portage_tmpdir = ""
		self.portdir = ""
		self.use = ""
		
		self.extra = {}
	
	def read(self):
		__tuple = self.reader.read_template("siissssssssssssi")
		(
			self.id,
			self.status,
			self.env_status,
			
			self.hostname,
			self.profile,
			self.arch,
			
			self.cflags,
			self.cxxflags,
			self.distdir,
			self.lc_messages,
			self.pkgdir,
			self.portage_logdir,
			self.portage_tmpdir,
			self.portdir,
			self.use,
			extra_n
		) = __tuple
		
		for i in range(extra_n):
			key = self.reader.read_string()
			self.extra[key] = self.reader.read_string()
		
		if self.reader.read_int() & 0xffffffff != AUTOGENTOO_HOST_END:
			raise IOError("Expected end of host")
	
	def get_path(self, sub=""):
		return "%s/%s/%s" % (self.parent.path, self.id, sub)


class Token:
	def __init__(self, reader):
		self.reader = reader
		
		self.auth_token = ""
		self.user_id = ""
		self.host_id = ""
		self.access_level = -1
	
	def read(self):
		(
			self.auth_token,
			self.user_id,
			self.host_id,
			self.access_level
		) = self.reader.read_template("sssi")
