import fcntl
import socket
import struct
import sys
from typing import Union, List, Tuple


class Streamable:
	def __init__(self, stream):
		self.template = ""
		self.stream = stream
	
	def read(self, n: int) -> bytes:
		return self.stream.read(n)
	
	def read_int(self) -> int:
		buf = self.read(4)
		if len(buf) != 4:
			return -1
		
		return struct.unpack('i', buf)[0]
	
	def read_str(self) -> Union[str, None]:
		length = self.read_int()
		
		if length == 0:
			return None
		elif length == -1:
			raise IOError("Failed to read 4 bytes from file")
		
		string = self.read(int(length))
		
		return string.decode("utf-8")
	
	def write(self, b: bytes) -> int:
		return self.stream.write(b)
	
	def write_int(self, i: int) -> int:
		return self.write(i.to_bytes(4, sys.byteorder))
	
	def write_str(self, string: str) -> int:
		out = self.write_int(len(string))
		return out + self.write(string.encode('utf-8'))
	
	def close(self):
		self.stream.close()
	
	@staticmethod
	def get_subtemplate(template, i) -> str:
		if template[i] != "(":
			ValueError("Template not at correct position expected '(' got '%s'" % template[i])
		
		buf_template = template[i + 1:]
		buf_pos = 0
		
		paren_level = 1
		
		while paren_level > 0:
			if buf_template[buf_pos] == "(":
				paren_level += 1
			elif buf_template[buf_pos] == ")":
				paren_level -= 1
			buf_pos += 1
		
		return buf_template[0:buf_pos - 1]
	
	def read_template(self, template) -> List[Union[int, str, List]]:
		out = []
		
		i = 0
		
		while i < len(template):
			if template[i] == 's':
				out.append(self.read_str())
			elif template[i] == 'i':
				out.append(self.read_int())
			elif template[i] == 'a':
				i += 1
				iter_count = self.read_int()
				sub_template = self.get_subtemplate(template, i)
				
				for j in range(iter_count):
					out.append(self.read_template(sub_template))
				
				i += len(sub_template)
			
			i += 1
		
		return out
	
	def write_list(self, lst: Union[List, Tuple]):
		for item in lst:
			if type(item) == str:
				self.write_str(item)
			elif type(item) == int:
				self.write_int(item)
			elif type(item) == dict:
				self.write_int(len(item))
				self.template += 'a(ss)'
				temp = self.template
				self.template = ""
				
				for key in item:
					self.write_str(key)
					self.write_str(item[key])
				
				self.template = temp
			elif type(item) in (tuple, list):
				self.write_int(len(item))
				self.template += 'a('
				temp = self.template
				self.template = ""
				for sub in item:
					self.write_list(sub)
				
				temp += self.template[:len(item[0])]
				self.template = temp + ")"
			elif issubclass(type(item), BinaryObject):
				item.write()
	
	def write_template(self, obj: Union[tuple, list], template: str) -> None:
		i = 0
		obj_i = 0
		
		while i < len(template):
			if template[i] == 'i':
				self.write_int(obj[obj_i])
			elif template[i] == 's':
				self.write_str(obj[obj_i])
			elif template[i] == 'a':
				i += 1
				sub_template = self.get_subtemplate(template, i)
				
				iter_num = len(obj[obj_i])
				
				self.write_int(iter_num)
				
				for j in obj[obj_i]:
					self.write_template(j, sub_template)
				
				i += len(sub_template)
			
			i += 1
			obj_i += 1
		
		self.template += template


class DynamicBinary(Streamable):
	def __init__(self, read_only=True):
		super().__init__(None)
		self.data = b""
		self.pos = 0
		
		self.read_only = read_only
		
		self.template = ""
	
	def write(self, buffer):
		if self.read_only:
			raise TypeError("This object is read only")
		
		self.data += buffer
		self.pos += len(buffer)
	
	def read(self, size):
		if not self.read_only:
			raise TypeError("This object is write only")
		
		outdata = self.data[self.pos:self.pos + size]
		self.pos += size
		
		if len(outdata) < size:
			raise IOError("Failed to read %d bytes from data" % size)
		
		return outdata
	
	def close(self):
		pass
	
	def read_int(self) -> int:
		return struct.unpack("!I", self.read(4))[0]
	
	def read_str(self) -> str:
		length = self.read_int()
		return struct.unpack("!%ds" % length, self.read(length))[0].decode("utf-8")
	
	def write_int(self, i, add_template=True) -> None:
		if add_template:
			self.template += "i"
		self.write(struct.pack("!I", i))
	
	def write_str(self, s: str) -> None:
		self.template += "s"
		self.write(struct.pack("!I%ds" % len(s), len(s), s.encode("utf-8")))
	
	def __str__(self):
		align = 12
		outstr = ""
		
		view = memoryview(self.data)
		
		for i in range(int(len(self.data) / align)):
			for byte in view[:align]:
				outstr += format(byte, "02x") + " "
			
			outstr += "\n"
			view = view[align:]
		
		for byte in view:
			outstr += format(byte, "02x") + " "
		
		return outstr


class FileReader(DynamicBinary):
	def __init__(self, path):
		super(FileReader, self).__init__()
		
		self.path = path
		self.file = None
		self.data = b""
	
	def read_data(self):
		self.file = open(self.path, "rb")
		fcntl.flock(self.file.fileno(), fcntl.LOCK_EX)
		
		self.data = self.file.read()
		self.template = ""
		self.pos = 0
		
		fcntl.flock(self.file.fileno(), fcntl.LOCK_UN)
		self.file.close()


class SockStream(DynamicBinary):
	def __init__(self, sock: socket.socket):
		super(SockStream, self).__init__()
		self.sock = sock
		self.data = b""
	
	def write(self, b: bytes) -> int:
		return self.sock.send(b)
	
	def read_data(self):
		block_size = 1024
		out = b""
		
		chunk = self.sock.recv(block_size)
		out += chunk
		while len(chunk) == block_size:
			chunk = self.sock.recv(block_size)
			out += chunk
		
		self.data = out


class BinaryObject:
	def __init__(self, reader: Streamable, template: str):
		self.reader = reader
		self.data = ()
		self.template = template
	
	def read(self):
		return self.reader.read_template(self.template)
	
	def write(self):
		self.reader.write_list(self.data)
