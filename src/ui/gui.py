#!/usr/bin/env python

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk

print("GTK VERSION %s.%s" % (Gtk.get_major_version(), Gtk.get_minor_version()))


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


class DscDialog(Dialog):
	def __init__(self, builder, name):
		Dialog.__init__(self, builder, name)
		
		self.template_select = self.builder.get_object("dnh_templates")
		self.template_entry = self.builder.get_object("dnh_template_new")
		self.profile = self.builder.get_object("dnh_profile")
		
		self.template_select.ad
	def ok(self, widget):
		super().ok(widget)
	
	def open(self, widget):
	


class ServerWindow(Gtk.Window):
	def __init__(self, builder):
		Gtk.Window.__init__(self, title="AutoGentoo server control")
		self.builder = builder
		
		self.top = builder.get_object("top")
		self.connect("destroy", Gtk.main_quit)
		self.add(self.top)
	
	def open_dialog(self, widget):
		dialog_linker = {
			"menu_file_new_host": DscDialog
		}
		
		name = Gtk.Buildable.get_name(widget)
		
		try:
			dialog_linker[name]
		except KeyError:
			print("%s is not handled by ServerWindow.open_dialog()" % name)
		else:
			dialog_linker[name](self.builder, dialog_linker[name])
	

def main(argc, args):
	server_builder = Gtk.Builder()
	server_builder.add_from_file("server.ui")
	server_gui = ServerWindow(server_builder)
	
	dialogs = {
		"new_host": DscDialog(server_builder, "dnh")
	}
	
	server_builder.connect_signals({
		"menu_file_new_host_activate_cb": dialogs["new_host"].open,
		"menu_file_open_activate_cb": server_gui.open_dialog,
		"menu_file_quit_activate_cb": server_gui.open_dialog,
		"dnh_close": dialogs["new_host"].close
	})
	
	server_gui.show_all()
	Gtk.main()


if __name__ == '__main__':
	import sys
	main(len(sys.argv), sys.argv)
