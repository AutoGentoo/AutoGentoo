#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  AutoGentoo.py
#  
#  Copyright 2015 Andrei Tumbar <atadmin@Kronos>
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

import os
import subprocess
import sys
from gi.repository import Gtk, Vte, GObject
from gi.repository import GLib
from gi.repository import GdkPixbuf
from gi.repository import Gdk
import platform
FileNotFoundError = IOError

#Variable Defaults
global timezone
timezone = "US/Eastern"
#cr stands for change root and will be used in the install functions
global cr
cr = "chroot /mnt/gentoo/ /bin/bash -c"

#Getting Download Location
def getarch():
	global arch
	#Finds current arch
	arch = platform.machine()
	#Since the platform.machine() function display as
	#x86_64 and so on I must change it to amd64
	#This is for the stage3 download and finding its location
	if arch == "x86_64":
		arch = "amd64"
	elif arch == "PA-RISC":
		arch = "hppa"
	elif arch == "Itanium":
		arch = "ia64"
	elif arch == "i686":
		arch = "x86"
	global line
	try:
		with open ("latest-stage3.txt", "r") as file:
			#This sends the latest stage3 file location to line
			line = file.read()
	except FileNotFoundError:
		findlatest = os.system("ls | grep 'latest-stage3.txt'")
		if findlatest != 0:
			getlatest = os.system("wget -q distfiles.gentoo.org/releases/%s/autobuilds/latest-stage3.txt" % (arch))
		try:
			with open ("latest-stage3.txt", "r") as file:
				line = file.read()
		except FileNotFoundError:
			#If we cant see the file the function will exit
			print("You are disconnected from the internet")
			print("AutoGentoo may not work properly!")
			return 0
	global stage3nameloc
	stage3nameloc = line[63:101]
	global stage3name
	stage3name = line[72:101]
	#Removes old file
	os.system("rm -rf latest-stage3.txt")
getarch()
#Install Functions
class install:
	def __init__(self):
		install.step_Download()
		install.step_extract()
		install.step_write_make_conf
		install.step_pre_update()
		install.step_update()
		install.step_x_server()
		install.step_optional() 
		install.step_write_config()
		install.step_bootloader()
		install.step_systemd(profilenum)
	def step_Download():
		widget.current_display.set_from_file(widget.download_img)
		top_level = builder.users.get_object("top_level")
		builder.builder.main_window.remove(top_level)
		toplevel_window = builder.download.get_object("top_level")
		toplevel_window.reparent(builder.main_window)
		builder.main_window.add(toplevel_window)
		builder.main_window.resize(727, 450)
		toplevel_window.attach(widget.current_display, 1, 1, 727, 409)
		widget.install_info.set_justify(Gtk.Justification.CENTER)
		widget.install_info.set_text("""Downloading Sources... 
From distfiles.gentoo.org/releases/%s/autobuilds/%s""" % (arch,stage3nameloc))
		check_gentoo = os.system("ls /mnt/gentoo")
		if check_gentoo == 2:
			os.system("mkdir /mnt/gentoo")
		os.system("wget -q --directory-prefix=/mnt/gentoo/ distfiles.gentoo.org/releases/%s/autobuilds/%s" % (arch,stage3nameloc))
	def step_extract():
		widget.current_display.set_from_file(widget.extract_img)
		if format_state == True:
			widget.install_info.set_text("""Formating Partitions...""")
			os.system("mkfs.ext2 -T small %s%s" % (genloc, part.boot))
			os.system("mkfs.ext4 -T small %s%s" % (genloc, part.root))
			if part.swap != 0:
				os.system("mkswap %s%s" % (genloc, part.swap))
				os.system("swapon %s%s" % (genloc, part.swap))
			if part.var != 0:
				os.system("mkfs.ext4 -T small %s%s" % (genloc, part.var))
			if part.usr != 0:
				os.system("mkfs.ext4 -T small %s%s" % (genloc, part.usr))
			if part.home != 0:
				os.system("mkfs.ext4 -T small %s%s" % (genloc, part.home))
			if part.opt != 0:
				os.system("mkfs.ext4 -T small %s%s" % (genloc, part.opt))
		widget.install_info.set_text("""Mounting Partitions...""" % (arch,stage3nameloc))
		os.system("mount %s%s /mnt/gentoo" % (genloc, part.root))
		check_boot_dir = os.system("ls /mnt/gentoo/boot")
		if check_boot_dir == 2:
			os.system("mkdir /mnt/gentoo/boot")
		if part.var != 0:
			check_var_dir = os.system("ls /mnt/gentoo/var")
			if check_var_dir == 2:
				os.system("mkdir /mnt/gentoo/var")
		if part.usr != 0:
			check_usr_dir = os.system("ls /mnt/gentoo/usr")
			if check_usr_dir == 2:
				os.system("mkdir /mnt/gentoo/usr")
		if part.home != 0:
			check_home_dir = os.system("ls /mnt/gentoo/home")
			if check_home_dir == 2:
				os.system("mkdir /mnt/gentoo/home")
		if part.opt != 0:
			check_opt_dir = os.system("ls /mnt/gentoo/opt")
			if check_opt_dir == 2:
				os.system("mkdir /mnt/gentoo/opt")
		os.system("mount %s%s /mnt/gentoo/boot" % (genloc, part.boot))
		if part.var != 0:
			os.system("mount %s%s /mnt/gentoo/var" % (genloc, part.var))
		if part.usr != 0:
			os.system("mount %s%s /mnt/gentoo/usr" % (genloc, part.usr))
		if part.home != 0:
			os.system("mount %s%s /mnt/gentoo/home" % (genloc, part.home))
		if part.opt != 0:
			os.system("mount %s%s /mnt/gentoo/opt" % (genloc, part.opt))
		widget.install_info.set_text("""Extracting Sources...""" % (arch,stage3nameloc))
		os.system("tar xjpvf /mnt/gentoo/%s -C /mnt/gentoo/ >> AutoGentoo.log" % (stage3name))
		check_proc = os.system("""mount | grep -i "/mnt/gentoo/proc" """)
		if check_proc == 1:
			os.system("mount -t proc /proc /mnt/gentoo/proc")
		check_dev = os.system("""mount | grep "/mnt/gentoo/dev" """)
		if check_dev == 1:
			os.system("mount --rbind /dev /mnt/gentoo/dev")
			os.system("mount --make-rslave /dev /mnt/gentoo/dev")
		check_sys = os.system("""mount | grep -i "/mnt/gentoo/sys" """)
		if check_sys == 1:
			os.system("mount --rbind /sys /mnt/gentoo/sys")
			os.system("mount --make-rslave /sys /mnt/gentoo/sys")
	def write_make_conf():
		if profile_num == 1:
			use_flags = "X -systemd -gnome -kde alsa sse sse2"
		elif profile_num == 2:
			use_flags = "X -systemd gnome -kde gtk -qt5 gtk3 alsa sse sse2"
		elif profile_num == 3:
			use_flags = "X systemd gnome -kde gtk -qt5 gtk3 alsa sse sse2"
		elif profile_num == 4:
			use_flags = "X -systemd -gnome kde qt5 gtk gtk3 alsa sse sse2"
		elif profile_num == 5:
			use_flags = "X systemd -gnome kde qt5 gtk gtk3 alsa sse sse2"
		elif profile_num == 6:
			use_flags = "X -systemd -gnome kde qt5 gtk gtk3 alsa sse sse2"
		elif profile_num == 7:
			use_flags = "X systemd -gnome kde qt5 gtk gtk3 alsa sse sse2"
		elif profile_num == 8:
			use_flags = "X -systemd -gnome -kde -qt5 -gtk -gtk3 alsa sse sse2"
		elif profile_num == 9:
			use_flags = "X systemd -gnome -kde -qt5 -gtk -gtk3 alsa sse sse2"
		if optimize == True:
			optimal = "-march=native"
		else:
			optimal = ""
		os.system("rm -rf /mnt/gentoo/etc/portage/make.conf")
		make_conf_file = open("/mnt/gentoo/etc/portage/make.conf", "w+")
		make_conf = ("""# These settings were set by the AutoGentoo build script that automatically
# built this stage.
# Please consult /usr/share/portage/config/make.conf.example for a more
# detailed example.
CFLAGS='%s -O2 -pipe'
CXXFLAGS='${CFLAGS}'
# WARNING: Changing your CHOST is not something that should be done lightly.
# Please consult http://www.gentoo.org/doc/en/change-chost.xml before changing.
CHOST='x86_64-pc-linux-gnu'
# These are the USE flags that were used in addition to what is provided by the
# profile used for building.
USE='%s'
VIDEO_CARDS='%s'
INPUT_DEVICES='evdev'
PORTDIR='/usr/portage ${PORTDIR_OVERLAY}'
DISTDIR='${PORTDIR}/distfiles'
PKGDIR='${PORTDIR}/packages' """ % (optimal, use_flags, gpu))
		make_conf_file.write(make_conf)
	def step_pre_update():
		widget.current_display.set_from_file(widget.pre_update_img)
		widget.download_top_level.attach(widget.current_display, 0, 0, 727, 409)
		widget.install_info.set_text("""Emerging Pre-Update Packages...""")
		os.system("cp -L /etc/resolv.conf /mnt/gentoo/etc 2> AutoGentoo.log")
		os.system("%s 'emerge-webrsync' >> AutoGentoo.log 2>&1" % (cr))
		os.system("%s 'emerge --sync' >> AutoGentoo.log 2>&1" % (cr))
		os.system("%s 'eselect profile set %s'" % (cr, profile))
		os.system("echo '%s' >> /mnt/gentoo/etc/timezone" % (timezone))
		os.system("%s 'emerge -q --config sys-libs/timezone-data' >> AutoGentoo.log" % (cr))
		os.system("echo '%s' >> /etc/locale.gen" % (locale))
		os.system("%s 'locale-gen' >> AutoGentoo.log" % (cr))
		os.system("%s 'source /etc/profile && env-update' >> AutoGentoo.log" % (cr))
		os.system("%s 'emerge -q linux-firmware' >> AutoGentoo.log" % (cr))
		os.system("%s 'emerge -q --autounmask-write devfsd' >> AutoGentoo.log 2>&1" % (cr))
		os.system("%s 'etc-update -q --automode -3' >> AutoGentoo.log 2>&1" % (cr))
		os.system("%s 'emerge -q devfsd' >> AutoGentoo.log" % (cr))
		os.system("%s 'emerge -q gentoo-sources' >> AutoGentoo.log" % (cr))
	def write_xdm(dm):
		xdmfile = open("/mnt/gentoo/etc/conf.d/xdm", "w")
		xdm = """# We always try and start X on a static VT. The various DMs normally default
	# to using VT7. If you wish to use the xdm init script, then you should ensure
	# that the VT checked is the same VT your DM wants to use. We do this check to
	# ensure that you haven't accidentally configured something to run on the VT
	# in your /etc/inittab file so that you don't get a dead keyboard.
	CHECKVT=7
	
	# What display manager do you use ?  [ xdm | gdm | kdm | gpe | entrance ]
	# NOTE: If this is set in /etc/rc.conf, that setting will override this one.
	DISPLAYMANAGER="%s"
	""" % dm
		xdmfile.write(xdm)
	def step_update():
		widget.current_display.set_from_file(widget.update_img)
		widget.download_top_level.attach(widget.current_display, 0, 0, 727, 409)
		widget.install_info.set_text("""Updating System...""")
		write_masks = os.system("%s 'emerge -qUDN --autounmask-write @world'" % cr)
		if write_masks != 0:
			os.system("%s 'dispatch-conf'" % cr)
			os.system("./scripts/update.sh")
	def step_x_server():
		widget.current_display.set_from_file(widget.x_servers_img)
		widget.download_top_level.attach(widget.current_display, 0, 0, 727, 409)
		widget.install_info.set_text("""Emerging X-Server...""")
		if profilenum == 1:
			os.system("emerge -q xorg-server >> AutoGentoo.log" % cr)
		elif profilenum == 2 or profilenum == 3:
				os.system("%s 'emerge -q --autounmask-write gnome' >> AutoGentoo.log 2>&1" % cr)
				os.system("%s 'dispatch-conf" % cr)
				os.system("%s 'emerge -q --autounmask-write libgdata' >> AutoGentoo.log 2>&1" % cr)
				os.system("%s 'dispatch-conf'" % cr)
				os.system("%s 'emerge -q dev-perl/SGMLSpm >> AutoGentoo.log 2>&1" % cr)
				os.system("%s 'echo 'dev-libs/openssl -bindist >> /etc/portage/package.use'" % cr)
				os.system("%s 'echo 'net-misc/openssh -bindist >> /etc/portage/package.use'" % cr)
				os.system("%s 'emerge -q openssh' >> AutoGentoo.log" % cr)
				os.system("%s 'emerge --autounmask-write -q gnome' >> AutoGentoo.log" % cr)
				os.system("%s 'dispatch-conf" % cr)
				os.system("%s 'emerge -q gnome' >> AutoGentoo.log" % cr)
		elif profilenum == 4 or profilenum == 5:
			check_kde = os.system("%s 'emerge -q --autoumask-write kde-base/kdebase-meta'" % cr)
			if check_kde != 0:
				os.system("dispatch-conf")
			os.system("%s 'emerge -q kde-base/kdebase-meta' >> AutoGentoo.log" % cr)
			os.system("%s 'emerge -q kdeplasma-addons' >> AutoGentoo.log" % cr)
		elif profilenum == 6 or profilenum == 7:
			os.system("%s 'emerge -q %s' >> AutoGentoo.log" % (cr, package.layman))
			os.system("%s 'layman --fetch --add kde'" % cr)
			os.system("%s 'ln -s /var/lib/layman/kde/Documentation/package.unmask/kde-plasma-5.3 /etc/portage/package.umask/" % cr)
			os.system("""%s 'echo "-qt5" >> /etc/portage/profile/use.stable.mask'""" % cr)
			os.system("%s 'ln -s /var/lib/layman/kde/Documentation/package.keywords/kde-frameworks-5.10.keywords /etc/portage/package.accept_keywords/'" % cr)
			os.system("%s 'ln -s /var/lib/layman/kde/Documentation/package.keywords/kde-plasma-5.3.keywords  /etc/portage/package.accept_keywords/'" % cr)
			os.system("%s 'emerge -q kde-plasma/plasma-meta' >> AutoGentoo.log" % cr)
			os.system("%s 'emerge -q dev-libs/sni-qt' >> AutoGentoo.log" % cr)
			os.system("%s 'emerge -q dev-libs/libappindicator:3' >> AutoGentoo.log" % cr)
			os.system("%s 'emerge -q dev-libs/libappindicator:2' >> AutoGentoo.log" % cr)
