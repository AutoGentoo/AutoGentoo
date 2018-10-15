from gi.repository import Gtk


class CheckValid(Gtk.Box):
	def __init__(self, handler, input_entry, error_template):
		super().__init__(orientation=Gtk.Orientation.HORIZONTAL)
		
		self.checking = Gtk.Spinner()
		self.worked = Gtk.Image.new_from_icon_name("gtk-ok", 8)
		self.errored = Gtk.Image.new_from_icon_name("gtk-no", 8)
		
		self.handler = handler
		self.input_entry = input_entry
		self.error_template = error_template
		self.current = None
	
	def clear(self):
		self.set_tooltip_text("")
		if self.current is not None:
			self.remove(self.current)
			self.current = None
	
	def add(self, widget):
		super().add(widget)
		self.current = widget
	
	def update(self):
		self.clear()
		self.add(self.spinner)
		
		text = self.input_entry.get_text()
		if len(text) == 0:
			self.clear()
			return
		
		if self.handler(text):
			self.clear()
			self.add(self.worked)
		else:
			self.clear()
			self.add(self.errored)
			self.set_tooltip_text(self.error_template % text)
