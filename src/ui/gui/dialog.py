class Dialog:
	def __init__(self, parent, builder, name):
		self.builder = builder
		self.dialog = builder.get_object(name)
		self.name = name
		self.parent = parent
		self.dialog.set_transient_for(self.parent)
		self.signals = {}
	
	def handle_signal(self, widget, name: str):
		signal_handle = name.replace("%s_" % self.name, "", 1)
		try:
			self.signals[signal_handle](widget)
		except KeyError:
			eval("self.%s(widget)" % signal_handle)
	
	def init_children(self, children):
		for x in children:
			self.init_child(x)
	
	def init_child(self, child_name):
		exec("self.%s = self.builder.get_object('%s_%s')" % (child_name, self.name, child_name))
	
	def close(self, widget=None):
		self.dialog.hide()
	
	def ok(self, widget=None):
		self.close()
	
	def open(self, widget=None):
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
			raise ObjectNotFoundError("Object '%s' could not be found in dialog register" % name)
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
