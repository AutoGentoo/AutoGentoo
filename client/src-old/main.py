#!/usr/bin/python3.4
# -*- coding: utf-8 -*-
#
#  main.py
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

import autogentoo
from ui import ui
import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk
from stdio import *

def main(args):
    server_main = autogentoo.Server ("kronos")
    ui_main = ui ("../ui/main.ui", server_main)
    ui_main.regen ()
    import signal    # enable Ctrl-C since there is no menu to quit
    signal.signal(signal.SIGINT, signal.SIG_DFL)

    Gtk.main ()
    return 0

if __name__ == '__main__':
    import sys
    sys.exit(main(sys.argv))