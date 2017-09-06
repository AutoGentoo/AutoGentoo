#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  packagemeta.py
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

from gi.repository import Gtk, Gdk, GObject, GdkPixbuf
import portage, sys

def css_rgba (r, b, g, a):
    return Gdk.RGBA(r/255,g/255,b/255,a)

class PackageMeta (Gtk.Box):
    __gtype_name__ = 'packageMeta'
    
    def __init__ (self):
        Gtk.Box.__init__ (self, orientation=Gtk.Orientation.VERTICAL)
        self.get_style_context ().add_class ("package-meta")
        
        self.header_top = Gtk.Box (orientation=Gtk.Orientation.HORIZONTAL)
        self.header_second = Gtk.Box (orientation=Gtk.Orientation.VERTICAL)
        self.level_view = packageCenter ("Available Versions")
        self.desc_top = Gtk.Box (orientation=Gtk.Orientation.VERTICAL)
        
        self.cat_label = Gtk.Label ()
        self.pkg_label = Gtk.Label ()
        self.desc_label = Gtk.Label ()
        self.homepkg_label = Gtk.LinkButton ()
        self.pkg_label.set_xalign (0.0)
        
        self.header_top.set_margin_bottom (12)
        self.header_top.set_margin_start (12)
        self.header_top.set_margin_top (12)
        
        self.override_background_color(Gtk.StateType.NORMAL, css_rgba(250, 250, 250, 1))
        
        self.header_second.pack_start (self.cat_label, True, False, 0)
        self.header_second.pack_start (self.pkg_label, True, False, 0)
        
        self.desc_top.pack_start (self.desc_label, False, False, 0)
        self.desc_top.pack_start (self.homepkg_label, False, False, 0)
        
        self.header_top.pack_start (self.header_second, False, False, 0)
        self.header_top.pack_start (self.desc_top, True, False, 0)
        self.pack_start (self.header_top, True, False, 0)
        self.pack_start (self.level_view, True, False, 0)
        
        self.img_buff = {
            "stable": "../ui/resources/stable.svg",
            "unstable": "../ui/resources/unstable.svg",
            "masked": "../ui/resources/masked.svg",
            "unknown": "../ui/resources/unknown.svg"
        }
        self.store = Gtk.ListStore (*([str] * 10))
        self.sorted_model = Gtk.TreeModelSort(model=self.store)
        self.sorted_model.set_sort_column_id(0, Gtk.SortType.DESCENDING)

        self.view = Gtk.TreeView.new_with_model(self.sorted_model)
        self.view.get_style_context ().add_class ("package-view")
        self.view.set_grid_lines (Gtk.TreeViewGridLines.BOTH)
        self.view.override_background_color(Gtk.StateType.NORMAL, css_rgba(255, 255, 255, 1))
        self.level_view.pack_start (self.view, True, False, 0)
        headers = ["amd64", "x86", "alpha", "arm", "hppa", "ia64", "ppc", "ppc64", "sparc"]
        
        sel = self.view.get_selection()
        sel.set_mode(Gtk.SelectionMode.NONE)

        
        renderer = Gtk.CellRendererText()
        column = Gtk.TreeViewColumn("Version", renderer, text=0)
        column.set_fixed_width (175)
        column.__index = 0
        column.set_cell_data_func(renderer, self.get_tree_cell_text)
        self.view.append_column(column)
        
        for i, column_title in enumerate(headers):
            px_renderer = Gtk.CellRendererPixbuf()
            px_column = Gtk.TreeViewColumn(column_title)
            px_column.set_fixed_width (57)
            px_column.__index = i+1
            px_column.pack_start(px_renderer, False)
            str_renderer = Gtk.CellRendererText()
            px_column.pack_start(str_renderer, False)
            # set data connector function/method
            px_column.set_cell_data_func(px_renderer, self.get_tree_cell_pixbuf)
            px_column.set_cell_data_func(str_renderer, self.get_tree_cell_text)
            px_column.spacing = 0
            self.view.append_column(px_column)
        
        self.meta = packageCenter ("Package Metadata")
        self.binaries = packageCenter ("Server Binaries")
        self.pack_start (self.meta, True, False, 0)
        self.pack_start (self.binaries, True, False, 0)
        
        self.show_all ()
        
    def _parse_package (self, _portage, package):
        self.store.clear()
        self.cat_label.set_markup ("<span font='Open Sans 20px' foreground='#777'>%s/</span>" % package.category)
        self.pkg_label.set_markup ("<span font='Open Sans 31px' foreground='#333'>%s</span>" % package.name)
        self.desc_label.set_markup ("<span font='Open Sans 31px' foreground='#333'>%s</span>" % package.description)
        self.homepkg_label.set_uri(package.homepage)
        self.homepkg_label.set_label(package.homepage)
        for ebuild in package.versions:
            buf = ["%s <span font='Open Sans 10px' foreground='#888'>: %s</span>" % (ebuild.id, ebuild.slot)]
            for arch in portage.current_keywords:
                imgbuf = self.img_buff[ebuild.keywords[arch]]
                buf.append (imgbuf)
                
            self.store.append (buf)
        
        self.use = useMeta (_portage, package)
        self.meta.pack_start(self.use, False, False, 0)
        self.license = licenseMeta (package)
        self.meta.pack_start (self.license, False, False, 0)

    def get_tree_cell_text(self, col, cell, model, iter, user_data):
        if (col.__index != 0):
            cell.set_fixed_size (0, 0)
        else:
            cell.set_property ('markup', cell.get_property ('text'))
        cell.set_padding (14, 0)
       
    def get_tree_cell_pixbuf(self, col, cell, model, iter, user_data):
        cell.set_padding (0, 0)
        cell.set_fixed_size (57, 35)
        cell.set_property('pixbuf', GdkPixbuf.Pixbuf.new_from_file_at_scale(model.get_value(iter, col.__index), 57, -1, True))


class packageCenter (Gtk.Box):
    __gtype_name__ = 'packageCenter'
    
    def __init__ (self, header):
        Gtk.Box.__init__ (self, orientation=Gtk.Orientation.HORIZONTAL)
        self.top = Gtk.Box (orientation=Gtk.Orientation.VERTICAL)
        self.head_label = Gtk.Label (header)
        self.top.get_style_context ().add_class ("package-center-top")
        self.set_can_focus (False)
        self.head_label.get_style_context ().add_class ("package-center-label")
        self.head_label.set_padding(15, 10)
        self.head_label.set_xalign (0.0)
        self.top.set_size_request (688, -1) # 688px is the allocated width of the TreeView
        self.set_margin_top (12)
        
        self.top.add (self.head_label)
        self._pack_start (self.top, True, False, 0)
    
    def _pack_start (self, widget, ex, fill, padding): # The super for Box
        Gtk.Box.pack_start (self, widget, ex, fill, padding)
    
    def pack_start (self, widget, ex, fill, padding):
        self.top.pack_start (widget, ex, fill, padding)

class iconLabel (Gtk.Box):
    __gtype_name__ = 'iconLabel'
    
    def __init__ (self, _icon_path, label_text):
        Gtk.Box.__init__ (self, orientation=Gtk.Orientation.VERTICAL)
        self.top = Gtk.Box (orientation=Gtk.Orientation.HORIZONTAL)
        self.set_margins (0, 15)
        self.icon = Gtk.Image.new_from_file (_icon_path)
        self.label = Gtk.Label ()
        self.label.set_markup (label_text)
        self.set_size_request (115, -1)
        self.label.set_xalign (0.0)
        
        self.top.pack_start (self.icon, True, False, 0)
        self.top.pack_start (self.label, True, True, 6)
        self.pack_start (self.top, False, True, 0)
        
    def set_margins (self, top, right=None, bottom=None, left=None):
        if (right == None):
            right = top
        if (bottom == None):
            bottom = top
        if (left == None):
            left = right
        if (left != -1):
            self.set_margin_start (left)
        if (right != -1):
            self.set_margin_end (right)
        if (top != -1):
            self.set_margin_top (top)
        if (bottom != -1):
            self.set_margin_bottom (bottom)

class inlineList (Gtk.Box):
    __gtype_name__ = 'inlineList'
    
    current_horizontal = 0
    max_horizontal = 0
    current_box = None
    
    def __init__ (self, max_horizontal):
        Gtk.Box.__init__ (self, orientation=Gtk.Orientation.VERTICAL) # Used for overflow
        self.max_horizontal = max_horizontal
        self.current_box = Gtk.Box (orientation=Gtk.Orientation.HORIZONTAL)
        self.add (self.current_box)
    
    def new_pack_start (self, widget, expand, fill, padding):
        self.current_box.pack_start (widget, expand, fill, padding) # Always append to last box
        self.current_horizontal += 1
        if (self.current_horizontal == self.max_horizontal):
            self.current_horizontal = 0
            self.current_box = Gtk.Box (orientation=Gtk.Orientation.HORIZONTAL)
            self.add (self.current_box)

class licenseMeta (Gtk.Box):
    __gtype_name__ = 'licenseMeta'
    
    def __init__ (self, package):
        Gtk.Box.__init__ (self, orientation=Gtk.Orientation.HORIZONTAL)
        self.set_margins (10, 15)
        
        self.left_side = iconLabel ("../ui/resources/legal.png", "License")
        self.pack_start (self.left_side, False, False, 0)
        
        self.right_side = Gtk.Box (orientation=Gtk.Orientation.VERTICAL)
        self.main_label = Gtk.Label ()
        self.main_label.get_style_context().add_class ('license')
        self.main_label.set_xalign (0.0)
        self.main_label.set_text (package.license)
        self.right_side.pack_start (self.main_label, True, True, 0)
        self.pack_start (self.right_side, False, False, 0)
        
    def set_margins (self, top, right=None, bottom=None, left=None):
        if (right == None):
            right = top
        if (bottom == None):
            bottom = top
        if (left == None):
            left = right
        if (left != -1):
            self.set_margin_start (left)
        if (right != -1):
            self.set_margin_end (right)
        if (top != -1):
            self.set_margin_top (top)
        if (bottom != -1):
            self.set_margin_bottom (bottom)

class useMeta (Gtk.Box):
    __gtype_name__ = 'useMeta'
    
    def __init__ (self, _portage, package):
        Gtk.Box.__init__ (self, orientation=Gtk.Orientation.HORIZONTAL)
        self.set_margins (10, 15)
        
        self.left_side = iconLabel ("../ui/resources/sliders.png", "USE flags")
        self.pack_start (self.left_side, False, False, 0)
        
        self.right_side = Gtk.Box (orientation=Gtk.Orientation.VERTICAL)
        self.local_use_label = Gtk.Label ()
        self.local_use_label.set_xalign (0.0)
        self.local_use_label.get_style_context ().add_class ("use-flag-info")
        self.local_use_label.set_text ("LOCAL USE FlAGS")
        self.global_use_label = Gtk.Label ()
        self.global_use_label.set_xalign (0.0)
        self.global_use_label.get_style_context ().add_class ("use-flag-info")
        self.global_use_label.set_text ("GLOBAL USE FlAGS")
        self.local_use = inlineList (6)
        self.global_use = inlineList (6)
        for x in _portage.local_use[package.category][package.name]:
            temp_l = Gtk.Label (x.name)
            temp_l.set_tooltip_text (x.description)
            self.local_use.new_pack_start (temp_l, False, False, 6)
        for x in package.globaluse:
            temp_l = Gtk.Label (_portage.global_use[x].name)
            temp_l.set_tooltip_text (_portage.global_use[x].description)
            self.global_use.new_pack_start (temp_l, False, False, 6)
        
        self.right_side.pack_start (self.local_use_label, False, False, 0)
        self.right_side.pack_start (self.local_use, False, False, 0)
        
        self.right_side.pack_start (self.global_use_label, False, False, 0)
        self.right_side.pack_start (self.global_use, False, False, 0)
        
        self.pack_start (self.right_side, False, False, 0)
    
    def set_margins (self, top, right=None, bottom=None, left=None):
        if (right == None):
            right = top
        if (bottom == None):
            bottom = top
        if (left == None):
            left = right
        if (left != -1):
            self.set_margin_start (left)
        if (right != -1):
            self.set_margin_end (right)
        if (top != -1):
            self.set_margin_top (top)
        if (bottom != -1):
            self.set_margin_bottom (bottom)
