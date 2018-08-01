from .dialog import Dialog
from gi.repository import Gtk
from .check_valid import CheckValid


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
				self.add(self.GenericItem(self.names[i]), placeholder=self.placeholders[i])
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
			self.value.set_completion(self.register)
		
		def is_valid_enum(self, text):
			try:
				int(text)
				return True
			except ValueError:
				try:
					self.register.get(text)
					return True
				except KeyError:
					return False


class DscDialog(Dialog):
	def __init__(self, builder, name):
		super().__init__(builder, name)
		
	
	def ok(self, widget):
		super().ok(widget)
	
	def open(self, widget):
		pass
