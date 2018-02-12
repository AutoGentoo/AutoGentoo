from collections import namedtuple
from typing import List, Tuple
selement = namedtuple('selement', ('type', 'name'))


class Struct:
	def __init__(self, elements: Tuple[selement], b_order='little'):
		self.el = {}
		self.order = elements
		self.b_order = b_order
		for e in elements:
			self.el[e.name] = selement(e.type, 0)
	
	def __getattr__(self, item):
		return self.el[item].value
	
	def __setattr__(self, key, value):
		self.el[key] = (self.el[key][0], value)
	
	def read(self, _bin):
		for el in self.order:
			if el.type == 'str':
				t = Struct.read_string(_bin)
				self.el[el.name] = selement(el.type, t)
				_bin = _bin[len(t):]
			elif el.type == 'int':
				self.el[el.name] = selement(el.type, self.read_int(_bin))
				_bin = _bin[4:]
			elif el.type.split(":")[0] == 'struct':
				exec("_bin = %s.read(_bin)" % el.type.split(":")[1])
		
		return _bin
	
	@staticmethod
	def read_string(_bin, _len=-1):
		i = 0
		if _len != -1:
			while i < _len:
				i += 1
		else:
			while _bin[i]:
				i += 1
		
		return str(_bin[:i])
	
	@staticmethod
	def sread_int(_bin, b_order):
		return int.from_bytes(_bin[:4], b_order)
	
	def read_int(self, _bin):
		return int.from_bytes(_bin[:4], self.b_order)
