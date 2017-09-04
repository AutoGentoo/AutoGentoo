#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  treegrid.py
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

from gi.repository import Gtk

class TreeGrid:
    __types = []
    __headers = []
    
    # GTK Objects
    treestore = None
    treeview = None
    treeview_select = None
    renderer = None
    select = None
    parent = None
    ex = -1
    
    def __init__ (self, *types):
        b = []
        for x in range(0, len(types)):
            t = types[x]
            if type(types[x]) in (tuple, list):
                assert self.ex > -1
                self.ex = x
                t = types[x][0]
            b.append (t)
        
        self.treestore = Gtk.TreeStore (*b)
        self.__types = types
        self.treeview = Gtk.TreeView.new_with_model(self.treestore)
        self.treeview_select = self.treeview.get_selection()
    
    def get_select (self):
        return self.treeview_select.get_selected ()[1] # Return treeiter of treeview
    
    def render (self, parent, headers=[], size=(700, 250)):
        self.__headers = headers
        self.parent = parent
        assert (len (self.__headers) == len(self.__types))
        for i, column_title in enumerate(self.__headers):
            self.renderer = Gtk.CellRendererText()
            column = Gtk.TreeViewColumn(column_title, self.renderer, text=i)
            self.treeview.append_column(column)
            if i == self.ex:
                self.treeview.set_expander_column(column)
        
        self.scrollable_treelist = Gtk.ScrolledWindow()
        self.select = self.treeview.get_selection()
        self.parent.set_vexpand(True)
        self.parent.add(self.treeview)
        self.parent.set_size_request(*size)
        self.parent.show_all ()

