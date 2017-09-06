#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  ui.py
#  
#  Copyright 2017 Andrei Tumbar <atuser@Hyperion>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#  
#  



import gi
gi.require_version('Vte', '2.91')
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, Vte, Gdk
from gi.repository.Vte import Terminal
from gi.repository import GLib
from treegrid import TreeGrid
import os
from socketrequest import SocketRequest
from stdio import *
from packagemeta import PortageMeta
import portage

class ui:
    builder = None
    file_p = None
    window = None
    server = None
    new_client = None # New client dialog
    handlers = {}
    
    clients_tree = None
    
    def __init__ (self, file_p, server):
        self.file_p = file_p
        self.server = server
        self.builder = Gtk.Builder()
        self.builder.add_from_file (self.file_p)
        self.clients_tree = TreeGrid (str, str, str, str, str, str)
        self.clients_tree.render (self.builder.get_object ("client_list_scroll"), ("ID", "Address", "Hostname", "Profile", "CHOST", "Active"))
        self.window = self.builder.get_object ("window_main")
        self.window.connect("delete-event", Gtk.main_quit)
        cpuinfo = self.builder.get_object ("_server_spec")
        ci_sr = SocketRequest (self.server.ip, self.server.port)
        res = "\n".join(ci_sr.send (b"SRV GETSPEC HTTP/1.0\n").decode ("utf-8").split ("\n")[:-2]) # Account for \n at end and HTTP/1.0 200 OK
        self.builder.get_object ("_server_spec_scroll").set_vexpand(True)
        ci_sr.close ()
        cpuinfo.set_text (res)
        self.portage = portage.portage (self.server.ip, "~/Downloads/portage")
        self.portageMeta = PortageMeta (self.portage)
        #self.portageMeta.parse_package (self.portage.packages["gnome-base"]["gnome"])
        self.portageMeta.parse_category ("gnome-base")
        self.builder.get_object ("_package_scroll").add (self.portageMeta)
        
        # CSS
        style_provider = Gtk.CssProvider()
        
        style_provider.load_from_path("../ui/css/main.css")

        Gtk.StyleContext.add_provider_for_screen(
            Gdk.Screen.get_default(),
            style_provider,
            Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION
        )
        
        # Dialogs
        self.new_client = self.builder.get_object ("dialog_new_client")
        
        self.handlers = {
            "client_change": self.client_change,
            "client_apply": self.client_apply,
            "sig_new_client": self.new_client_open,
            "new_client_close": self.new_client_close,
            "new_client_create": self.new_client_create,
            "remove_client_open": self.remove_client_open
        }
        self.builder.connect_signals (self.handlers)
        self.clients_tree.treeview.connect ("row-activated", self.activate)
        
        self.window.show()
    
    def remove_client_open (self, widget):
        model, _iter = self.clients_tree.treeview.get_selection ().get_selected ()
        _id = model.get_value (_iter, 0)
        dialog = Gtk.MessageDialog(self.window, 0, Gtk.MessageType.QUESTION,
            Gtk.ButtonsType.YES_NO, "Remove client with id '%s'" % _id)
        dialog.format_secondary_text(
            "Removing this client will remove all server side package and chroot environment")
        response = dialog.run()
        if response == Gtk.ResponseType.YES:
            remove_socket = SocketRequest (self.server.ip, self.server.port)
            remove_socket.send (("SRV SCREMOVE HTTP/1.0\n%s\n" % _id).encode ("utf-8"))
        dialog.destroy()
        self.regen()
    
    def new_client_close (self, widget):
        self.new_client.hide()
    
    def new_client_create (self, widget):
        new_entries = [
            self.builder.get_object ("new_hostname"),
            self.builder.get_object ("new_profile"),
            self.builder.get_object ("new_chost"),
            self.builder.get_object ("new_cflags"),
            self.builder.get_object ("new_use")
        ]
        args = []
        for x in new_entries:
            b = x.get_text ()
            if (b == ""):
                info_dialog = Gtk.MessageDialog(self.window, 0, Gtk.MessageType.INFO,
                    Gtk.ButtonsType.OK, "Not all required fields have values")
                info_dialog.format_secondary_text(
                    "Put valid values in to every (*) field")
                info_dialog.run()
                info_dialog.destroy()
                return
    
            args.append (x.get_text())
        
        start_iter = self.builder.get_object ("new_extra").get_start_iter()
        end_iter = self.builder.get_object ("new_extra").get_end_iter()
        extra_buff = self.builder.get_object ("new_extra").get_text(start_iter, end_iter, True).strip()
        if extra_buff:
            args.append (extra_buff)
        new_id = self.server.create (*args)
        self.regen (False)
        self.new_client_close (widget)
    
    def new_client_open (self, widget):
        new_entries = {
            self.builder.get_object ("new_hostname"): "",
            self.builder.get_object ("new_profile"): "default/linux/amd64/13.0/",
            self.builder.get_object ("new_chost"): "x86_64-pc-linux-gnu",
            self.builder.get_object ("new_cflags"): "-O2 -pipe",
            self.builder.get_object ("new_use"): "bindist mmx sse sse2",
            self.builder.get_object ("new_extra"): ""
        }
        
        for x, value in new_entries.items():
            x.set_text (value)
        
        self.new_client.show_all()
    
    def activate (self, treeview, path, column):
        __iter = treeview.get_model().get_iter (path)
        activate_socket = SocketRequest (self.server.ip, self.server.port)
        activate_socket.send (("SRV ACTIVATE HTTP/1.0\n%s\n" % treeview.get_model().get_value (__iter, 0)).encode ("utf-8"))
        self.server.get_active()
        self.regen (False)
    
    def regen (self, server_regen=True):
        if server_regen:
            self.server.regen ()
        self.clients_tree.treestore.clear()
        for i in range (len(self.server.clients)):
            client = self.server.clients[i]
            self.clients_tree.treestore.append (None, [client._id, self.server.ip, client.hostname, "...%s" % client.profile[-20:], client.CHOST, "•" if i == self.server.active else ""])
        self.gen_active ()
    
    def client_change (self, widget):
        if (self.server.active < 0):
            return
        active_client = self.server.clients[self.server.active]
        client_objs = {
            active_client._id: self.builder.get_object ("id"),
            active_client.CHOST: self.builder.get_object ("chost"),
            active_client.profile: self.builder.get_object ("profile"),
            active_client.hostname: self.builder.get_object ("hostname"),
            active_client.CFLAGS: self.builder.get_object ("cflags"),
            active_client.USE: self.builder.get_object ("use") # Do extras manually
        }
        needs_apply = 0
        for __str, obj in client_objs.items ():
            if (__str != obj.get_text ()):
                needs_apply = 1
                break # We dont need to continue after this
        start_iter = self.builder.get_object ("extra").get_start_iter()
        end_iter = self.builder.get_object ("extra").get_end_iter()
        if ("\n".join(active_client.EXTRAS) != self.builder.get_object ("extra").get_text(start_iter, end_iter, True)):
            needs_apply = 1
        if needs_apply:
            self.builder.get_object ("_client_apply").set_sensitive(True)
        else:
            self.builder.get_object ("_client_apply").set_sensitive(False)
    
    def client_apply (self, widget):
        hostname = self.builder.get_object ("hostname").get_text()
        profile = self.builder.get_object ("profile").get_text()
        chost = self.builder.get_object ("chost").get_text()
        cflags = self.builder.get_object ("cflags").get_text()
        use = self.builder.get_object ("use").get_text()
        start_iter = self.builder.get_object ("extra").get_start_iter()
        end_iter = self.builder.get_object ("extra").get_end_iter()
        extras = self.builder.get_object ("extra").get_text(start_iter, end_iter, True).split ("\n")
        
        edit_socket = SocketRequest (self.server.ip, self.server.port)
        res = edit_socket.send (("SRV EDIT %s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n" % (
            len(extras), 
            self.server.clients[self.server.active]._id, 
            hostname,
            profile,
            chost,
            cflags,
            use,
            "\n".join (extras)
        )).encode ("utf-8")).decode ("utf-8")
        self.regen ()
    
    def gen_active (self):
        if (self.server.active < 0):
            self.builder.get_object ("_client_hostname").set_label ("Activate a client")
            self.builder.get_object ("id").set_text ("")
            self.builder.get_object ("chost").set_text ("")
            self.builder.get_object ("profile").set_text ("")
            self.builder.get_object ("hostname").set_text ("")
            self.builder.get_object ("cflags").set_text ("")
            self.builder.get_object ("use").set_text ("")
            self.builder.get_object ("extra").set_text ("")
            return
        active_client = self.server.clients[self.server.active]
        self.builder.get_object ("_client_hostname").set_label (active_client.hostname)
        self.builder.get_object ("id").set_text (active_client._id)
        self.builder.get_object ("chost").set_text (active_client.CHOST)
        self.builder.get_object ("profile").set_text (active_client.profile)
        self.builder.get_object ("hostname").set_text (active_client.hostname)
        self.builder.get_object ("cflags").set_text (active_client.CFLAGS)
        self.builder.get_object ("use").set_text (active_client.USE)
        self.builder.get_object ("extra").set_text ("\n".join(active_client.EXTRAS))
