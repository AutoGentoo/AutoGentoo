import struct
from typing import Union


class DynamicBinary:
	def __init__(self, read_only=True):
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
		
		return outdata
	
	def read_int(self):
		return struct.unpack("!I", self.read(4))[0]
	
	def read_string(self):
		length = self.read_int()
		return struct.unpack("!%ds" % length, self.read(length))[0].encode("utf-8")
	
	def write_int(self, i):
		self.write(struct.pack("!I", i))
	
	def write_string(self, s):
		self.write(struct.pack("!I%ds" % len(s), len(s), s))
	
	@staticmethod
	def get_subtemplate(template, i):
		if template[i] != "(":
			ValueError("Template not at correct position expected '(' got '%s'" % template[i])
		
		buf_template = template[i + 1:]
		buf_pos = 0
		
		paren_level = 1
		
		while paren_level:
			if buf_template[buf_pos] == "(":
				paren_level += 1
			elif buf_template[buf_pos] == ")":
				paren_level -= 1
		
		return buf_template[0:buf_pos - 1]
	
	def read_template(self, template):
		out = []
		
		i = 0
		
		while i < len(template):
			if template[i] == 's':
				out.append(self.read_string())
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
	
	def write_template(self, obj: Union[tuple, list], template: str):
		i = 0
		obj_i = 0
		
		while i < len(template):
			if template[i] == 'i':
				self.write_int(obj[obj_i])
			elif template[i] == 's':
				self.write_string(obj[obj_i])
			elif template[i] == 'a':
				i += 1
				sub_template = self.get_subtemplate(template, i)
				
				self.write_template(obj[obj_i], sub_template)
				
				i += len(sub_template)
			
			i += 1
			obj_i += 1
