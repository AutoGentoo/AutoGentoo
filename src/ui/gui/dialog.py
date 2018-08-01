class Dialog:
	def __init__(self, builder, name):
		self.builder = builder
		self.dialog = builder.get_object(name)
		self.name = name
	
	def close(self, widget):
		self.dialog.hide()
	
	def ok(self, widget):
		self.close(widget)
	
	def open(self, widget):
		self.dialog.run()


class ObjectNotFoundError(Exception):
	pass


class ObjectExistsError(Exception):
	pass


class DialogRegister:
	def __init__(self, init_dict=None):
		if init_dict is None:
			init_dict = {}
		
		self.table = init_dict
	
	def get_object(self, name):
		try:
			self.table[name]
		except KeyError:
			raise ObjectNotFoundError("Object '%s' could not be found in %s register")
		else:
			return self.table[name]
	
	def add_object(self, name, gtk_object, overwrite=False):
		try:
			self.table[name]
		except KeyError:
			self.table[name] = gtk_object
		else:
			if overwrite:
				old = self.table[name]
				self.table[name] = gtk_object
				return old
			raise ObjectExistsError("Object with name '%s' exists and overwrite has not been enabled")
