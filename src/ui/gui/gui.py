#!/usr/bin/env python

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk
from dialog import *
from dialog_send_custom import DscDialog

print("GTK VERSION %s.%s" % (Gtk.get_major_version(), Gtk.get_minor_version()))


class Dialogs:
	SEND_CUSTOM = 1
	NEW_HOST = 2
	NEW_SERVER = 3


class ServerWindow(Gtk.Window):
	def __init__(self, builder):
		super().__init__(title="AutoGentoo server control")
		self.builder = builder
		self.dialogs = DialogRegister({
			Dialogs.SEND_CUSTOM: DscDialog(self, self.builder),
			# Dialogs.NEW_HOST: DnhDialog(self, self.builder)
		})
		
		self.dialog_signal_links = {
			"dsc": Dialogs.SEND_CUSTOM
		}
		
		self.top = builder.get_object("top")
		self.connect("destroy", Gtk.main_quit)
		self.add(self.top)
	
	def dialog(self, dialog_enum) -> Dialog:
		return self.dialogs.get_object(dialog_enum)
	
	def open_dialog(self, dialog_enum):
		self.dialog(dialog_enum).open()
	
	def handle_save_as(self, widget):
		pass
	
	def handle_save(self, widget):
		pass
	
	def handle_open(self, widget):
		pass
	
	def handle_dialog_signal(self, widget, name):
		self.dialog(self.dialog_signal_links[name[0:name.find("_")]]).handle_signal(widget, name)


def main(argc, args):
	server_builder = Gtk.Builder()
	server_builder.add_from_file("server.ui")
	server_gui = ServerWindow(
		server_builder)
	
	server_builder.connect_signals({
		"server_new": lambda x: server_gui.open_dialog(Dialogs.NEW_SERVER),
		"server_send_custom": lambda x: server_gui.open_dialog(Dialogs.SEND_CUSTOM),
		"server_quit": Gtk.main_quit,
		"server_save_as": server_gui.handle_save_as,
		"server_save": server_gui.handle_save,
		"server_open": server_gui.handle_open,
		
		"dsc_close": lambda widget: server_gui.handle_dialog_signal(widget, "dsc_close"),
		"dsc_control_add": lambda widget: server_gui.handle_dialog_signal(widget, "dsc_control_add"),
		"dsc_structs_change": lambda widget: server_gui.handle_dialog_signal(widget, "dsc_structs_change"),
		"dsc_ok": lambda widget: server_gui.handle_dialog_signal(widget, "dsc_ok"),
		
		# "dnh_close": server_gui.dialog(Dialogs.NEW_HOST).handle_signal,
	})
	
	server_gui.show_all()
	Gtk.main()


if __name__ == '__main__':
	import sys
	main(len(sys.argv), sys.argv)
