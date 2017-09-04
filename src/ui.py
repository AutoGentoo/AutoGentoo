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
        self.clients_tree = TreeGrid (str, str, str)
        self.clients_tree.render (self.builder.get_object ("client_list_scroll"), ("ID", "Address", "Hostname"))
        self.window = self.builder.get_object ("window_main")
        self.window.connect("delete-event", Gtk.main_quit)
        terminal = self.builder.get_object ("_server_ssh")
        terminal.spawn_sync(
            Vte.PtyFlags.DEFAULT,
            os.environ['HOME'],
            ["/usr/bin/ssh", "root@%s" % self.server.ip],
            [],
            GLib.SpawnFlags.DO_NOT_REAP_CHILD,
            None,
            None,
        )
        clients_view = self.builder.get_object ("clients_view")
        
        self.window.show_all()
