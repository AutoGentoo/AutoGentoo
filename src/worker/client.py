from autogentoo_api.dynamic_binary import FileReader, BinaryObject

AUTOGENTOO_ACCESS_TOKEN = 0xdddddddd
AUTOGENTOO_HOST = 0xfffffff0
AUTOGENTOO_SERVER_TOKEN = 0xfffff000
AUTOGENTOO_HOST_END = 0xaaaaaaaa
AUTOGENTOO_FILE_END = 0xffffffff
AUTOGENTOO_SUDO_TOKEN = 0xcccccccc
AUTOGENTOO_HOST_ID_LENGTH = 16


class Server(BinaryObject):
	def __init__(self, path, parent_pid):
		self.path = path
		self.parent_pid = parent_pid
		self.file = "%s/.autogentoo.config" % self.path
		
		reader = FileReader(self.file, parent_pid)
		
		super(Server, self).__init__(reader, "")
		
		self.hosts = {}
		self.keys = {}
		
		self.sudo_token = None
		self.autogentoo_org_token = None
		self.data = ()

	def read_host(self):
		host = Host(self.reader, self)
		host.read()
		self.hosts[host.id] = host
	
	def read_token(self):
		token = Token(self.reader)
		token.read()
		self.keys[token.auth_token] = token
	
	def read(self):
		self.reader.read_data()

		self.keys = {}
		self.hosts = {}
		
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
	
	def write(self):
		self.data = (
			*list(self.hosts.values()),
			*list(self.keys.values()),
			AUTOGENTOO_SERVER_TOKEN,
			self.autogentoo_org_token,
			AUTOGENTOO_SUDO_TOKEN,
			self.sudo_token,
			AUTOGENTOO_FILE_END
		)
		
		self.reader.start_write()
		super().write()
		self.reader.end_write()


class Host(BinaryObject):
	def __init__(self, reader: FileReader, parent):
		super(Host, self).__init__(reader, "siissssssssssssi")

		self.parent = parent
		
		self.id = ""
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
		self.data = ()
		
		self.extra = {}
	
	def read(self):
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
		) = super().read()
		
		for i in range(extra_n):
			key = self.reader.read_string()
			self.extra[key] = self.reader.read_string()
		
		if self.reader.read_int() & 0xffffffff != AUTOGENTOO_HOST_END:
			raise IOError("Expected end of host")
	
	def extra_keys(self):
		return [{"k": k, "v": v} for k, v in self.extra.items()]
	
	@staticmethod
	def generate_id():
		import random
		out_str = ""
		for i in range(AUTOGENTOO_HOST_ID_LENGTH):
			out_str += "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"[int(random.random()*62)]
		
		return out_str
	
	def write(self):
		self.data = (
			AUTOGENTOO_HOST,
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
			self.extra,
			AUTOGENTOO_HOST_END
		)
		
		super().write()
	
	def get_path(self, sub=""):
		return "%s/%s/%s" % (self.parent.path, self.id, sub)


class Token(BinaryObject):
	def __init__(self, reader):
		super(Token, self).__init__(reader, "sssi")
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
		) = super().read()
	
	def write(self):
		self.data = (
			AUTOGENTOO_ACCESS_TOKEN,
			self.auth_token,
			self.user_id,
			self.host_id,
			self.access_level
		)
		
		super().write()
