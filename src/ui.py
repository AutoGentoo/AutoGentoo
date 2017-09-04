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
from gi.repository import Gtk, Vte
from gi.repository.Vte import Terminal
from gi.repository import GLib
from treegrid import TreeGrid
import os
from socketrequest import SocketRequest
from stdio import *

class ui:
    builder = None
    file_p = None
    window = None
    server = None
    
    clients_tree = None
    
    def __init__ (self, file_p, server):
        self.file_p = file_p
        self.server = server
        self.builder = Gtk.Builder()
        self.builder.add_from_file (self.file_p)
        self.clients_tree = TreeGrid (str, str, str, str, str)
        self.clients_tree.render (self.builder.get_object ("client_list_scroll"), ("ID", "Address", "Hostname", "Profile", "CHOST"))
        self.window = self.builder.get_object ("window_main")
        self.window.connect("delete-event", Gtk.main_quit)
        cpuinfo = self.builder.get_object ("_server_spec")
        ci_sr = SocketRequest (self.server.ip, self.server.port)
        res = "\n".join(ci_sr.send (b"SRV GETSPEC HTTP/1.0\n").decode ("utf-8").split ("\n")[:-2]) # Account for \n at end and HTTP/1.0 200 OK
        self.builder.get_object ("_server_spec_scroll").set_vexpand(True)
        ci_sr.close ()
        cpuinfo.set_text (res)
        self.window.show_all()
        
    def regen (self):
        self.server.regen ()
        for client in self.server.clients:
            self.clients_tree.treestore.append (None, [client._id, self.server.ip, client.hostname, "...%s" % client.profile[-20:], client.CHOST])
        self.gen_active ()
    
    def gen_active (self):
        if (self.server.active < 0):
            return
        active_client = self.server.clients[self.server.active]
        self.builder.get_object ("_client_hostname").set_label (active_client.hostname)
        self.builder.get_object ("id").set_text (active_client._id)
        self.builder.get_object ("chost").set_text (active_client.CHOST)
        self.builder.get_object ("hostname").set_text (active_client.hostname)
        self.builder.get_object ("cflags").set_text (active_client.CFLAGS)
        self.builder.get_object ("use").set_text (active_client.USE)
        self.builder.get_object ("extra").set_text ("\n".join(active_client.EXTRAS))
