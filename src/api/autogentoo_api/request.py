from dynamic_binary import *
import socket
import collections
import ssl
from typing import List


Address = collections.namedtuple("Address", ("ip", "port"))
RequestStruct = collections.namedtuple('RequestStruct', 'struct_type args')


class Client:
	def __init__(self, adr: Address, _ssl=True):
		self.address = adr
		self.ssl = _ssl
		
		self.context = ssl.create_default_context()
		
		self.socket = socket.socket(socket.AF_INET)
		self.conn = None
	
	def connect(self):
		if self.ssl:
			self.conn = self.context.wrap_socket(self.socket, server_hostname=self.address.ip)
		else:
			self.conn = self.socket
		
		self.conn.connect(tuple(self.address))
	
	def send(self, data: DynamicBinary):
		self.conn.sendall(data.data)
	
	def recv(self) -> DynamicBinary:
		bufsize = 128
		buf = self.socket.recv(bufsize)
		
		out = buf
		
		while len(buf) == bufsize:
			buf = self.socket.recv(bufsize)
			out += buf
		
		outptr = DynamicBinary()
		outptr.data = out
		
		return outptr


class Request:
	(
		REQ_GET,
		REQ_HEAD,
		REQ_POST,
		REQ_START,
		
		REQ_HOST_NEW,
		REQ_HOST_EDIT,
		REQ_HOST_DEL,
		REQ_HOST_EMERGE,
		
		REQ_HOST_MNTCHROOT,
		REQ_SRV_INFO,
		REQ_SRV_REFRESH,
		REQ_AUTH_ISSUE_TOK,
		REQ_AUTH_REFRESH_TOK,
		REQ_AUTH_REGISTER,
		REQ_JOB_STREAM,
		REQ_HOST_STAGE3,
		
		REQ_MAX
	) = range(17)
	
	(
		STRCT_END,
		STRCT_HOST_NEW,
		STRCT_HOST_SELECT,
		STRCT_HOST_EDIT,
		STRCT_AUTHORIZE,
		STRCT_EMERGE,
		STRCT_ISSUE_TOK,
		STRCT_JOB_SELECT,
		
		STRCT_MAX
	) = range(9)
	
	request_args = {
		REQ_HOST_NEW: (STRCT_AUTHORIZE, STRCT_HOST_NEW),
		REQ_HOST_EDIT: (STRCT_AUTHORIZE, STRCT_HOST_SELECT, STRCT_HOST_EDIT),
		REQ_HOST_DEL: (STRCT_AUTHORIZE, STRCT_HOST_SELECT),
		REQ_HOST_EMERGE: (STRCT_AUTHORIZE, STRCT_HOST_SELECT, STRCT_EMERGE),
		REQ_HOST_MNTCHROOT: (STRCT_AUTHORIZE, STRCT_HOST_SELECT),
		REQ_SRV_INFO: (),
		REQ_SRV_REFRESH: (STRCT_AUTHORIZE,),
		REQ_AUTH_ISSUE_TOK: (STRCT_AUTHORIZE, STRCT_HOST_SELECT, STRCT_ISSUE_TOK),
		REQ_AUTH_REFRESH_TOK: (STRCT_AUTHORIZE,),
		REQ_AUTH_REGISTER: (STRCT_AUTHORIZE, STRCT_ISSUE_TOK),
		REQ_JOB_STREAM: (STRCT_AUTHORIZE, STRCT_HOST_SELECT, STRCT_JOB_SELECT),
		REQ_HOST_STAGE3: (STRCT_AUTHORIZE, STRCT_HOST_SELECT, STRCT_JOB_SELECT)
	}
	
	request_structure_linkage = (
		"sss",  # /* Host new */
		"s",  # /* Host select */
		"iss",  # /* Host edit */
		"ss",  # /* Host authorize */
		"s",  # /* Emerge arguments */
		"ssi",  # /* Issue Token */
		"s",  # /* Job select */
	)
	
	@staticmethod
	def host_new(arch: str, profile: str, hostname: str) -> RequestStruct:
		return RequestStruct(struct_type=Request.STRCT_HOST_NEW, args=(arch, profile, hostname))
	
	# request_type 1: make_conf 2: general
	@staticmethod
	def host_edit(request_type, make_conf_var: str, make_conf_val: str) -> RequestStruct:
		return RequestStruct(struct_type=Request.STRCT_HOST_EDIT, args=(request_type, make_conf_var, make_conf_val))

	@staticmethod
	def host_select(hostname: str) -> RequestStruct:
		return RequestStruct(struct_type=Request.STRCT_HOST_SELECT, args=(hostname,))

	@staticmethod
	def authorize(user_id: str, token: str) -> RequestStruct:
		return RequestStruct(struct_type=Request.STRCT_AUTHORIZE, args=(user_id, token))

	@staticmethod
	def host_emerge(emerge_str: str) -> RequestStruct:
		return RequestStruct(struct_type=Request.STRCT_EMERGE, args=(emerge_str,))

	@staticmethod
	def issue_token(user_id: str, target_host: str, access_level) -> RequestStruct:
		return RequestStruct(struct_type=Request.STRCT_ISSUE_TOK, args=(user_id, target_host, access_level))
	
	@staticmethod
	def job_select(job_id: str) -> RequestStruct:
		return RequestStruct(struct_type=Request.STRCT_JOB_SELECT, args=(job_id,))
	
	def __init__(self, adr: Address, request: int, structs: List[RequestStruct], _ssl=True):
		self.address = adr
		self.ssl = _ssl
		
		self.request = request
		self.structs = structs
		
		for i, st in enumerate(self.structs):
			if st.struct_type != self.request_args[self.request][i]:
				raise TypeError("Invalid argument expected '%d' got '%d'" % (self.request_args[self.request][i], st.struct_type))
		
		self.data = DynamicBinary(read_only=False)
		
		self.data.write(b'\x00')
		self.data.write_int(self.request)
		
		for st in self.structs:
			self.data.write_int(st.struct_type)
			self.data.write_template(st.args, self.request_structure_linkage[st.struct_type])
		
		self.data.write_int(self.STRCT_END)
		
		self.client = Client(self.address, self.ssl)
		
		self.code = -1
		self.message = ""
		self.content = None
	
	def send(self):
		self.client.connect()
		self.client.send(self.data)
	
	def recv(self):
		outdata = self.client.recv()
		
		self.code = outdata.read_int()
		self.message = outdata.read_string()
		
		template = outdata.read_string()
		
		if template is None:
			return []
		
		self.content = outdata.read_template(template)
