from dynamic_binary import *
import socket
import collections
from OpenSSL import SSL
from typing import List, Union, Tuple


Address = collections.namedtuple("Address", "ip port")
RequestStruct = collections.namedtuple('RequestStruct', 'struct_type args')


class Client:
	def __init__(self, adr: Address, _ssl=True):
		self.address = adr
		self.ssl = _ssl
		
		self.socket = socket.socket(socket.AF_INET)
		self.context = SSL.Context(SSL.SSLv23_METHOD)
	
	def connect(self):
		if self.ssl:
			self.socket = SSL.Connection(self.context, self.socket)
		
		self.socket.connect(tuple(self.address))
		
		if self.ssl:
			self.socket.do_handshake()
	
	def close(self):
		self.socket.close()
	
	def send(self, data: DynamicBinary):
		self.socket.sendall(data.data)
	
	def recv(self, maxsize=-1) -> DynamicBinary:
		if maxsize != -1:
			buf = self.socket.recv(maxsize)
			outptr = DynamicBinary()
			outptr.data = buf
			
			return outptr
		
		bufsize = 128
		buf = bytearray(bufsize)
		view = memoryview(buf)
		
		outdata = b""
		
		nbytes = self.socket.recv_into(view, bufsize)
		
		while nbytes > 0:
			outdata += view[:nbytes]
			
			try:
				nbytes = self.socket.recv_into(view, bufsize)
			except SSL.SysCallError:
				break
			except BrokenPipeError:
				break
		
		outptr = DynamicBinary()
		outptr.data = outdata
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
	
	ARGS = {
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
	
	LINKAGE = (
		"",
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
	
	def __init__(self, adr: Address, request: int, structs: Union[List[RequestStruct], Tuple[RequestStruct]], _ssl=True):
		self.address = adr
		self.ssl = _ssl
		
		self.request = request
		self.structs = structs
		
		for i, st in enumerate(self.structs):
			if st.struct_type != Request.ARGS[self.request][i]:
				raise TypeError("Invalid argument expected '%d' got '%d'" % (Request.ARGS[self.request][i], st.struct_type))
		
		self.data = DynamicBinary(read_only=False)
		
		self.data.write(b'\x00')
		self.data.write_int(self.request)
		
		for st in self.structs:
			self.data.write_int(st.struct_type)
			self.data.write_template(st.args, Request.LINKAGE[st.struct_type])
		
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
		
		self.client.close()
		
		self.code = outdata.read_int()
		self.message = outdata.read_string()
		
		template = outdata.read_string()
		
		if template is None:
			return None, -1, "Connection Error"
		
		self.content = outdata.read_template(template)
		
		return self.content, self.code, self.message
