import struct
from typing import Union, List, Tuple
import _thread
import signal
import os


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
		
		if len(outdata) < size:
			print(self.pos)
			print(outdata)
			raise IOError("Failed to read %d bytes from data" % size)
		
		return outdata
	
	def read_int(self) -> int:
		return struct.unpack("!I", self.read(4))[0]
	
	def read_string(self) -> str:
		length = self.read_int()
		return struct.unpack("!%ds" % length, self.read(length))[0].decode("utf-8")
	
	def write_int(self, i, add_template=True) -> None:
		if add_template:
			self.template += "i"
		self.write(struct.pack("!I", i))
	
	def write_string(self, s: str) -> None:
		self.template += "s"
		self.write(struct.pack("!I%ds" % len(s), len(s), s.encode("utf-8")))

	def write_list(self, lst: Union[List, Tuple]):
		for item in lst:
			if type(item) == str:
				self.write_string(item)
			elif type(item) == int:
				self.write_int(item)
			elif type(item) == dict:
				self.write_int(len(item), False)
				self.template += 'a(ss)'
				temp = self.template
				self.template = ""
				
				for key in item:
					self.write_string(key)
					self.write_string(item[key])
				
				self.template = temp
			elif type(item) in (tuple, list):
				self.write_int(len(item), False)
				self.template += 'a('
				temp = self.template
				self.template = ""
				for sub in item:
					self.write_list(sub)
				
				temp += self.template[:len(item[0])]
				self.template = temp + ")"
			elif issubclass(type(item), BinaryObject):
				item.write()

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
	
	def write_template(self, obj: Union[tuple, list], template: str) -> None:
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
				
				iter_num = len(obj[obj_i])
				
				self.write_int(iter_num)
				
				for j in obj[obj_i]:
					self.write_template(j, sub_template)
				
				i += len(sub_template)
			
			i += 1
			obj_i += 1
		
		self.template += template
	
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
	def __init__(self, path, parent_pid):
		super(FileReader, self).__init__()
		
		self.parent_pid = parent_pid
		self.path = path
		self.file = None
		self.data = b""
		
		self.config_lck = _thread.allocate_lock()
		self.ack_lck = _thread.allocate_lock()
		signal.signal(signal.SIGUSR1, self.handle_toggle)
		signal.signal(signal.SIGUSR2, self.handle_ack)
	
	def handle_ack(self, _signum, _stack):
		self.ack_lck.release()
	
	def handle_toggle(self, _signum, _stack):
		if self.config_lck.locked():
			self.config_lck.release()
		else:
			self.config_lck.acquire()
	
	def read_data(self):
		self.file = open(self.path, "rb")
		self.data = self.file.read()
		self.pos = 0
		self.file.close()
	
	def toggle_server_lock(self):
		self.ack_lck.acquire()
		os.kill(self.parent_pid, signal.SIGUSR1)
		
		# WAIT FOR SIGUSR2
		
		self.ack_lck.acquire()
		self.ack_lck.release()
	
	def start_write(self):
		self.data = b""
		self.pos = 0
		
		self.config_lck.acquire()
		self.toggle_server_lock()
		
	def stop_write(self):
		self.file = open(self.path, "wb+")
		self.file.write(self.data)
		self.file.flush()
		self.file.close()
		
		self.config_lck.release()
		self.toggle_server_lock()


class BinaryObject:
	def __init__(self, reader: FileReader, template: str):
		self.reader = reader
		self.data = ()
		self.template = template
	
	def read(self):
		return self.reader.read_template(self.template)
	
	def write(self):
		self.reader.write_list(self.data)
