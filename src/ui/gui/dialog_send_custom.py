from dialog import Dialog
from gi.repository import Gtk
from check_valid import CheckValid


class DscCustomStruct(Gtk.Box):
	def __init__(self, template, names, placeholders, enum_register):
		super().__init__(orientation=Gtk.Orientation.VERTICAL, spacing=6)
		self.template = template
		self.names = names
		self.enum_register = enum_register
		self.placeholders = placeholders
		self.parse_template()
	
	def parse_template(self):
		template_i = 0
		i = 0
		while template_i < len(self.template):
			_type = self.template[template_i]
			if _type == 'i':
				self.add(self.EnumItem(self.names[i], self.enum_register))
			elif _type == 's':
				self.add(self.GenericItem(self.names[i], placeholder=self.placeholders[i]))
			elif _type == 'a':
				template_i += 2
				old_i = template_i
				
				paren_index = 1
				while paren_index != 0:
					if self.template[template_i] == '(':
						paren_index += 1
					elif self.template[template_i] == ')':
						paren_index -= 1
					template_i += 1
				
				self.add(self.ArrayItem(
					self.template[old_i:template_i],
					self.names[i],
					self.placeholders[i],
					self.enum_register))
			i += 1
			template_i += 1
	
	class GenericItem(Gtk.Box):
		def __init__(self, name, custom_value=None, placeholder=""):
			super().__init__(orientation=Gtk.Orientation.HORIZONTAL)
			self.name = name
			
			self.label = Gtk.Label("%s:" % self.name)
			self.value = Gtk.Entry() if custom_value is None else custom_value
			
			self.add(self.label)
			self.add(self.value)
			if custom_value is None:
				self.value.set_placeholder_text(placeholder)
	
	class ArrayItem(GenericItem):
		def __init__(self, inner_template, inner_names, inner_placeholders, enum_register):
			self.array_box = DscCustomStruct(
				inner_template,
				inner_names,
				inner_placeholders,
				enum_register)
			super().__init__(inner_names[0], self.array_box)
	
	class EnumItem(GenericItem):
		def __init__(self, name, register):
			super().__init__(name, placeholder="int|enum")
			
			self.register = register
			self.valid = CheckValid(self.is_valid_enum, self.value, "%s is not a valid enum or integer")
			self.value.set_completion(self.register.completion)
			self.int_value = -1
		
		def __int__(self):
			return self.int_value
		
		def is_valid_enum(self, text):
			try:
				int(text)
				return True
			except ValueError:
				try:
					self.int_value = self.register[text]
					return True
				except KeyError:
					return False


class EnumRegister(Gtk.Notebook):
	def __init__(self, file_parse):
		super().__init__()
		
		self.file = open(file_parse, "r")
		self.lines = self.file.readlines()
		self.registry_table = {}
		self.pages = []
		self.completion = Gtk.EntryCompletion()
		
		self.parse()
		self.generate_completion()
		self.show_all()
	
	def generate_completion(self):
		liststore = Gtk.ListStore(str)
		for enum in self.registry_table:
			liststore.append([enum])
		self.completion.set_model(liststore)
		self.completion.set_text_column(0)
		
	def append_page(self, name, enum_list):
		# page = Gtk.TreeStore()
		# page.set_border_width(10)
		
		# self.append_page(page, Gtk.Label(name))
		# self.pages.append(page)
		pass
	
	def get(self, enum_name):
		return self.registry_table[enum_name]
	
	def parse(self):
		i = 0
		while i < len(self.lines):
			if self.lines[i][0] == '-':
				current_file = self.lines[i][1:]
				file_enum = {}
				i += 1
				while i < len(self.lines) and self.lines[i][0] != '-':
					current = 0
					current_enum = {}
					while self.lines[i][0] != '}':
						do_eval = False
						splt = [y.strip() for y in self.lines[i].split("=")]
						if len(splt) == 2:
							current = splt[1].replace(",", "")
							do_eval = True
						current_enum[splt[0]] = current
						self.registry_table[splt[0]] = current
						if do_eval:
							current = eval(current)
						current += 1
						i += 1
					c_line = self.lines[i]
					file_enum[c_line[2:c_line.find(";")]] = current_enum
					i += 1
				self.append_page(current_file, file_enum)
			else:
				i += 1


class DscDialog(Dialog):
	def __init__(self, parent, builder):
		super().__init__(parent, builder, "dsc")
		
		self.init_children([
			"content_box",
			"left_paned",
			"struct_box",
			"structs_combo"
		])
		
		self.enums = EnumRegister("enums.txt")
		self.content_box.pack2(self.enums)
		self.structs = []
		self.struct_current = None
		
		init_structs = [
			("host_edit", "iis", ("Select one", "Select two", "Field"), ("", "", ""))
		]
		
		for x in init_structs:
			self.add_struct(*x)
	
	def add_struct(self, name, template, field_names, placeholders):
		id_index = len(self.structs)
		self.structs.append(DscCustomStruct(template, field_names, placeholders, self.enums))
		self.structs_combo.append(str(id_index), name)
	
	def structs_change(self, widget):
		to_add = self.structs[int(self.structs_combo.get_active())]
		print(to_add)
		if to_add == self.struct_current:
			return
		
		if self.struct_current is not None:
			self.struct_box.remove(self.struct_current)
		
		self.struct_box.pack_end(to_add, True, True, 6)
		self.struct_current = to_add
		to_add.show_all()
