#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  AutoGentoo.py
#  
#  <Graphical gentoo installer>
#  Copyright 2015 Andrei Tumbar <atadmin@Kronos>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
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

import os, sys, time, subprocess, platform
from gi.repository import Gtk, Vte, GObject, GLib, Pango
from src.partitions.stepPart import *
from html.parser import HTMLParser

global top_level
class defaults:
	#Variable to find the install type
	#Will be set to default value of 'default'
	install_type = "default"
	#Sets whether the system will update when installing
	#install_type 'default' requires updates to be installed
	update = True
	default_unit = "m"
	original_unit = "m"
	original_size = 0
	current_root_size_unit = "m"
	root_passwd = ""
	hostname = ""
	username = ""
	user_passwd = ""
	user_passwd_cfm = ""
	driver_cpu_selected = 0
	optimize = False 
#Builder deals with gtk ui configurations for each screen or window
class builder:
	disk_type = diskType("/dev/sda")
	main = Gtk.Builder()
	main.add_from_file("gtk/stepMain.ui")
	main_window = main.get_object("main")
	main_window.set_name("main1")
	main_window.set_icon_from_file('img/optional.png')
	current = main
	current_step = 1
	
	#Partition Window Configuration
	ask_part = Gtk.Builder()
	ask_part.add_from_file("gtk/stepAsk.ui")
	css = open('gtk/style.css')
	css_data = css.read()
	css.close()
	style_provider = Gtk.CssProvider()
	style_provider.load_from_path('gtk/style.css')
	Gtk.StyleContext.add_provider_for_screen(
		Gdk.Screen.get_default(), style_provider,
		Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION
	)
	
	#Advanced Partitions Window Configuration
	adv_part = Gtk.Builder()
	adv_part.add_from_file("gtk/stepPartAdvanced.ui")
	dialog_new_part = adv_part.get_object("dialog-new-part-gpt")
	
	#Xserver Window Configuration 
	xserver = Gtk.Builder()
	xserver.add_from_file("gtk/stepXServer.ui")
	
	#Gnome display manager Configuration
	gnome = Gtk.Builder()
	gnome.add_from_file("gtk/stepGNOME.ui")
	
	#KDE display manager Configuration
	kde = Gtk.Builder()
	kde.add_from_file("gtk/stepKDE.ui")
	
	#xfce display manager Configuration
	xfce = Gtk.Builder()
	xfce.add_from_file("gtk/stepXFCE.ui")
	
	#Portage Binhost Configuration
	binhost = Gtk.Builder()
	binhost.add_from_file("gtk/stepBinHost.ui")
	
	#Variables Window Configuration
	var = Gtk.Builder()
	var.add_from_file("gtk/stepVariables.ui")
	
	#Default partition size
	rootPart = Gtk.Builder()
	rootPart.add_from_file("gtk/stepSetPartSize.ui")
	
	#User Info Window Configuration
	user = Gtk.Builder()
	user.add_from_file("gtk/stepUserInfo.ui")
	#Newpartition window
	are_you_sure = adv_part.get_object("are_you_sure_win")
	make_table = adv_part.get_object("change_part_table_win")
	edit_part = adv_part.get_object("change_part_win")
	add_package_win = var.get_object("add_package")
	
	#Install Window Configuration
	install = Gtk.Builder()
	install.add_from_file("gtk/stepInstall.ui")
#Change directory to /usr/lib/autogentoo

class hrefParser(HTMLParser):
	def handle_starttag(self, tag, attrs):
		for attr in attrs:
			if attr[0] == "href":
				self.link = attr[1]
	def handle_data(self, data):
		if not data == '\r\n':
			self.name = data
class stringParser(HTMLParser):
	data = []
	def handle_data(self, data):
		self.data.append(data)
class gpu():
	vendor = ""
	product_full = ""
	product_id = ""
	product = ""
	vendor_class = ""
	driver_version = ""
	working_driver_version = ""
	version_list = []
	echo_list = []
	def __init__(self):
		gpu.vendor = ""
		gpu.product_full = ""
		gpu.product_id = ""
		gpu.product = ""
		gpu.vendor = str(subprocess.check_output("lshw -class display | grep vendor", shell=True))
		gpu.vendor = gpu.vendor[15:]
		gpu.vendor = gpu.vendor.replace("\n", "")
		gpu.product_full = str(subprocess.check_output("lshw -class display | grep product", shell=True))
		gpu.product_full = gpu.product_full[16:]
		gpu.product_id_temp = []
		current_char = ""
		current_char_num = -1
		while current_char != "[":
			current_char_num += 1
			current_char = gpu.product_full[current_char_num]
			gpu.product_id_temp.append(current_char)
		gpu.product_id = str1 = str(''.join(gpu.product_id_temp))
		gpu.product_id = gpu.product_id.replace(" ", "")
		gpu.product_id = gpu.product_id.replace("[", "")
		gpu.product = gpu.product_full.replace("%s" % gpu.product_id, "")
		gpu.product = gpu.product.replace("[", "")
		gpu.product = gpu.product.replace("]", "")
		gpu.product = gpu.product.replace("\n", "")
		gpu.product = gpu.product.replace(" ", "", 1)
		print ("Found %s gpu" % gpu.vendor)
		print ("Model: %s\nIdentification: %s" % (gpu.product, gpu.product_id))
		if gpu.vendor.find("NVIDIA") != -1:
			gpu.vendor_class = "nvidia"
		else:
			gpu.vendor_class = "radeon"
		if gpu.vendor_class == "nvidia":
			get_version = get_nvidia_versions()
			gpu.version_list = get_version[0]
			os.system("rm -rf ./graphics/nvidia/nvidia*")
			get_nvidia_supported()
			get_echo()
			version_list = gpu.version_list
			find_supported = 1 # Placeholder
			current_version = -1
			while find_supported != 0:
				current_version += 1
				find_supported = os.system("cat ./graphics/nvidia/nvidia-%s.supported_cards | grep -i '%s' > /dev/null" % (version_list[current_version], gpu.product))
			print ("Found supported version %s" % version_list[current_version])
			print ("Echoing version %s" % gpu.echo_list[current_version]) 
			gpu.driver_version = version_list[current_version]
			gpu.working_driver_version = gpu.driver_version
		elif gpu.vendor_class == "radeon":
			gpu.driver_version = "x11-drivers/radeon-ucode"
			print ("Found supported version %s" % gpu.driver_version)
			gpu.working_driver_version = gpu.driver_version
def get_nvidia_versions():
	os.system("wget -q 'http://www.nvidia.com/object/unix.html'")
	name_list = []
	href_list = []
	return_list = []
	unix_file = open("unix.html", "r").readlines()
	for x in unix_file:
		if x.find("/AMD64/") != -1:
			amd_line = x
			break
	x = unix_file.index(amd_line)
	parser = hrefParser()
	for line in range(x, x+7):
		parser.feed(unix_file[line+1])
		if parser.link[0] == "/":
			parser.link = "http://www.nvidia.com%s" % parser.link
		name_list.append(parser.name)
		href_list.append(parser.link)
	return_list.append(name_list)
	return_list.append(href_list)
	os.system("rm -rf unix.html") 
	return return_list
def get_nvidia_supported():
	returned = get_nvidia_versions()
	link_list = returned[1]
	name_list = returned[0]
	for x in link_list:
		os.system("wget -q -O curr_file.html %s" % x)
		curr_file = open("curr_file.html", "r").readlines()
		os.system("rm -rf curr_file.html")
		for y in curr_file:
			if y.find("tab2_content") != -1:
				tab_line = curr_file.index(y)
				break
		data_line = curr_file[tab_line+1]
		classdata = stringParser()
		classdata.feed(data_line)
		data = '\n'.join(classdata.data)
		link_num = link_list.index(x)
		file_name = "nvidia-%s.supported_cards" % name_list[link_num]
		file = open("graphics/nvidia/%s" % file_name, "w+")
		file.write(data)
		file.close()
def get_echo():
	for x in gpu.version_list:
		for y in x:
			if y == ".":
				num = x.index(y)
				gpu.echo_list.append(x[:num])
				break
class get_arch:
	stage3_location = ""
	architecture = ""
	stage3name = ""
	def __init__(self):
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
		try:
			with open ("latest-stage3.txt", "r") as file:
				#This sends the latest stage3 file location to line
				line = file.read()
		except FileNotFoundError:
			findlatest = os.system("ls | grep 'latest-stage3.txt'")
			if findlatest != "latest-stage3.txt":
				getlatest = os.system("wget -q distfiles.gentoo.org/releases/%s/autobuilds/latest-stage3.txt" % (arch))
			try:
				with open ("latest-stage3.txt", "r") as file:
					line = file.read()
			except FileNotFoundError:
				#If we cant see the file the function will exit
				print("You are disconnected from the internet")
				print("AutoGentoo may not work properly!")
				return 1
		
		get_arch.stage3_location = line[63:101]
		get_arch.architecture = arch
		get_arch.stage3name = line[72:101]
		#Removes old file
		os.system("rm -rf latest-stage3.txt")
def get_newpart():
	if builder.disk_type == "gpt":
		#GPT
		builder.dialog_new_part = builder.adv_part.get_object("dialog-new-part-gpt")
	else:
		#MBR
		builder.dialog_new_part = builder.adv_part.get_object("dialog-new-part-mbr")
get_newpart()
class makeopts:
	use_flags = ""
	cflags = ""
class xserver:
	xserver = "none"
	display_manager = "startx"
class systype:
	ins_custom = True
	gendev = ""
	rootSize = 0
	gpu_type = ""
class current:
	part = ""
	size = 0
	label = 0
	primary = True
	start_of_free = True
	fstype = "ext2"
	fs_id = 1
	formatyn = False
	mount_point = ""
	unit = "M"
	start = 0
	end = 0
class get_emerge:
	emerge_list = []
	def calculation_process(self):
		os.system("echo -ne 'Calculating Atom List")
		while True:
			os.system("echo -ne .")
			os.system("sleep .5s")
			os.system("echo -ne .")
			os.system("sleep .5s")
			os.system("echo -ne .")
			os.system("sleep .5s")
			os.system("echo -ne '\r\r\r   '")
			os.system("sleep .5s")
	def __init__(self, atom):
		os.system("emerge -qp %s >> temp_emerge" % atom)
class package:
	list = []
class widget:
	check_update = builder.ask_part.get_object("check_updates")
	part_size = builder.rootPart.get_object("partSize")
	set_disk = builder.adv_part.get_object("gentoo_device")
	partitions = Gtk.TreeStore(str, str, str, str, str, str, str)
	partition_grid = builder.adv_part.get_object("part_grid")
	root_disk = builder.rootPart.get_object("rootDisk")
	treeview = Gtk.TreeView.new_with_model(partitions)
	for i, column_title in enumerate(["Partition", "Filesystem", "Partition Type", "Size", "Mount Point", "Start", "End"]):
		renderer = Gtk.CellRendererText()
		column = Gtk.TreeViewColumn(column_title, renderer, text=i)
		treeview.append_column(column)
		if i == "Partition":
			treeview.set_expander_column(column)
	scrollable_treelist = Gtk.ScrolledWindow()
	select = treeview.get_selection()
	grub_device = builder.adv_part.get_object("grub_device")
	add_part = builder.adv_part.get_object("add_part")
	remove_part = builder.adv_part.get_object("remove_part")
	change_part = builder.adv_part.get_object("change_partition")
	part_size_adj = builder.adv_part.get_object("part_size_adj")
	mount_point_change = ""
	gentoo_dev_info = builder.adv_part.get_object("gentoo_device_info")
	part_label = builder.adv_part.get_object("part_label")
	format_type = builder.adv_part.get_object("format_type")
	format_type1 = builder.adv_part.get_object("format_type1")
	partition_edit_format_checkbutton = builder.adv_part.get_object("partition_edit_format_checkbutton")
	partition_edit_format_checkbutton1 = builder.adv_part.get_object("partition_edit_format_checkbutton1")
	mount_point1 = builder.adv_part.get_object("mount_point1")
	mount_point2 = builder.adv_part.get_object("mount_point2")
	table_type = builder.adv_part.get_object("table_type")
	format_type_change = builder.adv_part.get_object("format_type_change")
	mount_point_change = builder.adv_part.get_object("mount_point_change")
	loading_spinner = builder.adv_part.get_object("loading_spinner")
	loading_spinner.set_visible(False)
	recalculating = builder.adv_part.get_object("recalculating")
	memory_bar = builder.var.get_object("memory_bar")
	memory_label = builder.var.get_object("memory_label")
	desktop_manager = builder.var.get_object("desktop_manager")
	dm_state = builder.var.get_object("dm_state")
	install_desktop = builder.var.get_object("install_desktop")
	enable_custom_packages = builder.var.get_object("enable_custom_packages")
	packages = Gtk.ListStore(str, str)
	treeview_pkg = Gtk.TreeView.new_with_model(packages)
	for i, column_title_pkg in enumerate(["Package", "USE Flags"]):
		renderer_pkg = Gtk.CellRendererText()
		column_pkg = Gtk.TreeViewColumn(column_title_pkg, renderer, text=i)
		treeview_pkg.append_column(column_pkg)
	scrollable_treelist_pkg = Gtk.ScrolledWindow()
	select_pkg = treeview_pkg.get_selection()
	delete_pkg = builder.var.get_object("remove")
	top_level_pkg = builder.var.get_object("top_level_packages")
	packages.append(["sys-process/htop", ""])
	packages.append(["app-admin/sudo", ""])
	packages.append(["app-portage/gentoolkit", ""])
	packages.append(["sys-block/parted", ""])
	packages.append(["dev-python/pyparted", "python_targets_python2_7 python_targets_python3_4 python_targets_python3_5"])
	packages.append(["sys-apps/dmidecode", ""])
	packages.append(["sys-fs/e2fsprogs", ""])
	packages.append(["sys-fs/dosfstools", ""])
	packages.append(["sys-fs/ntfs3g", ""])
	
	package.list.append(["sys-process/htop"])
	package.list.append(["app-admin/sudo"])
	package.list.append(["app-portage/gentoolkit"])
	package.list.append(["sys-block/parted"])
	package.list.append(["sys-apps/dmidecode"])
	pkg_temp = []
	pkg_temp_str = ""
	new_package_win = builder.var.get_object("new_package")
	pkg_name = builder.var.get_object("pkg_name")
	root_strength = builder.user.get_object("password_strength1")
	hostname_ok = builder.user.get_object("hostname_ok")
	hostname_error_label = builder.user.get_object("hostname_error_label")
	username_ok = builder.user.get_object("username_ok")
	username_error_label = builder.user.get_object("username_error_label")
	password_strength = builder.user.get_object("password_strength")
	password_ok = builder.user.get_object("password_ok")
	password_error_label = builder.user.get_object("password_error_label")
	main_unit = builder.adv_part.get_object("main_unit")
	gpu_type_info = builder.var.get_object("gpu_type_info")
	gpu_version_info = builder.var.get_object("gpu_version_info")
	gpu_version_change_nvidia = builder.var.get_object("gpu_version_change_nvidia")
	start_part = builder.adv_part.get_object("start_part")
	end_part = builder.adv_part.get_object("end_part")
	radeon_drivers = builder.var.get_object("radeon_drivers")
	gpu_grid = builder.var.get_object("gpu_grid")
	driver_warning = builder.var.get_object("driver_warning")
	optimize_warning = builder.var.get_object("optimize_warning")
	add_packages = builder.var.get_object("add_packages")
	cflags_window = builder.var.get_object("cflags_window")
	custom_cflags = builder.var.get_object("custom_cflags")
	custom_cflags_button = builder.var.get_object("custom_cflags_button")
	custom_cflags_buffer = builder.var.get_object("custom_cflags_buffer")
	install_info = builder.install.get_object("install_info")
	install_top_level = builder.install.get_object("top_level")
	scrollwindow_terminal = builder.install.get_object("scrollwindow_terminal")
	terminal = Vte.Terminal()
	#terminal.spawn_sync(Vte.PtyFlags.DEFAULT, os.environ['HOME'], ["/usr/bin/autogentoolog"], [], GLib.SpawnFlags.DO_NOT_REAP_CHILD, None, None)
	scrollwindow_terminal.add(terminal)
	logical = builder.adv_part.get_object("partition_create_type_logical")
	primary = builder.adv_part.get_object("partition_create_type_primary")
	format_iter = builder.adv_part.get_object("format_iter")
class mounts:
	part = []
	mount_point = []
	fstype = []
class config:
	localegen = """# /etc/locale.gen: list all of the locales you want to have on your system
#
# The format of each line:
# <locale> <charmap>
#
# Where <locale> is a locale located in /usr/share/i18n/locales/ and
# where <charmap> is a charmap located in /usr/share/i18n/charmaps/.
#
# All blank lines and lines starting with # are ignored.
#
# For the default list of supported combinations, see the file:
# /usr/share/i18n/SUPPORTED
#
# Whenever glibc is emerged, the locales listed here will be automatically
# rebuilt for you.  After updating this file, you can simply run `locale-gen`
# yourself instead of re-emerging glibc.

en_US ISO-8859-1"
en_US.UTF-8 UTF-8"
ja_JP.EUC-JP EUC-JP"
ja_JP.UTF-8 UTF-8"
ja_JP EUC-JP"
en_HK ISO-8859-1"
en_PH ISO-8859-1"
de_DE ISO-8859-1"
de_DE@euro ISO-8859-15"
es_MX ISO-8859-1"
fa_IR UTF-8"
fr_FR ISO-8859-1"
fr_FR@euro ISO-8859-15"
it_IT ISO-8859-1"""
	makeconf = """# These settings were set by the catalyst build script that automatically
# built this stage.
# Please consult /usr/share/portage/config/make.conf.example for a more
# detailed example.
CFLAGS='%s'
CXXFLAGS='${CFLAGS}'
# WARNING: Changing your CHOST is not something that should be done lightly.
# Please consult http://www.gentoo.org/doc/en/change-chost.xml before changing.
CHOST='x86_64-pc-linux-gnu'
# These are the USE flags that were used in addition to what is provided by the
# profile used for building.
USE='%s'
VIDEO_CARDS='%s'
INPUT_DEVICES='evdev synaptics'
PORTDIR='/usr/portage'
DISTDIR='${PORTDIR}/distfiles'
PKGDIR='${PORTDIR}/packages'
PORTDIR_OVERLAY=/usr/local/portage""" % (makeopts.cflags, makeopts.use_flags, gpu.vendor_class)
def do_packages ( ):
	os.system ( "emerge \"%s\" > temp.cfg" % ' '.join ( package.list ) )
def write_config ( ):
	os.system ("curl --output /dev/null --silent --head --fail")
	write_out = []
	write_out.append ( "[config]" )
	write_out.append ( "profile=%s" % defaults.install_type )
	write_out.append ( "hostname=%s" % defaults.hostname )
	write_out.append ( "root_passwd=%s" % defaults.root_passwd )
	write_out.append ( "username=%s" % defaults.username )
	write_out.append ( "user_passwd=%s" % defaults.user_passwd )
	write_out.append ( "updates=%s" % str ( defaults.update ).lower ( ) )
	write_out.append ( "gentoo_device=%s" % systype.gendev )
	write_out.append ( "[partitions]" )
	if ( defaults.install_type == "default" ):
		write_out.append ( "rootSize=%s" % systype.gendev )
	else:
		os.system ( "mount | grep \"/mnt/gentoo\" > mount.info" )
		mounts = open ( "mount.info", "r" ).readlines ( )
		for part in mounts:
			part.replace ( " on", "" )
			part.replace ( " type", "" )
			part = part [ 0:part.find ( "(" - 1 ) ]
			inf = part.split ( " " )
			write_out.append ( "%s=%s" % ( inf [ 0 ], inf [ 2 ] ) )
	write_out.append ( "[packages]" )
	curr_dir = os.path.dirname(os.path.realpath(__file__))
	write_out.append ( "opt=%s/temp.cfg" % curr_dir )
	write_out.append ( "[gui]" )
	write_out.append ( "gpu=%s" % gpu.vendor )
	write_out.append ( "download=http://us.download.nvidia.com/XFree86/Linux-{0}/{1}/NVIDIA-Linux-{0}-{1}.run".format ( platform.machine(), gpu.working_driver_version ) )
	write_out.append ( "xserver=%s" % xserver.xserver )
	write_out.append ( "manager=%s" % xserver.xserver )
def do_part_first(disk_num, unit="MiB"):
	disk()
	find_disk = disk.disks[disk_num]
	part(find_disk, "MiB")
	iters = []
	print (len(widget.treeview))
	for x in range(0, len(part.paths)):
		curr_iter = widget.partitions.append(get_iter(widget.partitions, iters, part.parent[x]), 
			[part.paths[x], 
			part.fileSystem[x], 
			part.type[x], 
			"%s%s" % (part.size[x], unit),
			part.mounts[x], 
			"%s%s" % (part.start[x], unit), 
			"%s%s" % (part.end[x], unit)])
		iters.append(curr_iter)
	widget.scrollable_treelist.set_vexpand(True)
	widget.partition_grid.attach(widget.scrollable_treelist, 0, 0, 1, 1)
	widget.scrollable_treelist.add(widget.treeview)
	widget.scrollable_treelist.set_size_request(715, 250)
	widget.scrollable_treelist.show_all()
	disk()
	curr_remove = -1
	while curr_remove != len(disk.disks):
		curr_remove += 1
		widget.set_disk.remove(0)
	print ("\n Number of Disks: %s" % len(disk.disks))
	for x in range(0, len(disk.disks)):
		disk()
		widget.set_disk.insert(x, str(x), disk.disks[x])
	widget.set_disk.set_active(disk_num)
def do_part(disk_num, unit="MiB"):
	print("Doing Part...")
	if disk == None:
		print ("No disk selected")
		return
	#Show the loading bar
	widget.recalculating.set_label("Recalculating Partitions...")
	widget.loading_spinner.set_visible(True)
	widget.loading_spinner.start()
	widget.recalculating.set_visible(True)
	#Check if disk exists
	disk()
	find_disk = disk.disks[disk_num]
	if not os.path.exists(find_disk):
		print ("This disk has been removed")
		widget.loading_spinner.set_visible(False)
		widget.recalculating.set_visible(False)
		widget.loading_spinner.stop()
		widget.set_disk.set_active(disk_num-1)
		widget.set_disk.remove(disk_num)
		return
	#Clear old partitions
	widget.partitions.clear()
	part(find_disk, unit)
	model = widget.grub_device.get_model()
	model.clear()
	iters = []
	for x in range(0, len(part.paths)):
		curr_iter = widget.partitions.append(get_iter(widget.partitions, iters, part.parent[x]),
			[part.paths[x],
			part.fileSystem[x],
			part.type[x],
			"%s%s" % (part.size[x], unit),
			part.mounts[x],
			"%s%s" % (part.start[x], unit),
			"%s%s" % (part.end[x], unit)])
		iters.append(curr_iter)
	widget.scrollable_treelist.show_all()
	for x in part.partitions:
		if x.type == 0 and x.getLength("MiB") >= 100:
			print ("Passed")
			widget.grub_device.insert(int(part.paths.index(x.path)), "%s" % x.path, "%s (%s)" % (x.path, round(x.getLength("MiB"), 2)))
	widget.gentoo_dev_info.set_text("Gentoo Device (%s)" % (diskType(find_disk)))
	widget.loading_spinner.set_visible(False)
	widget.recalculating.set_visible(False)
	widget.loading_spinner.stop()
	widget.treeview.expand_all()
	print ("Done!")
def do_disk():
	get_sel = widget.set_disk.get_active()
	disk()
	curr_remove = -1
	while curr_remove != len(disk.disks):
		curr_remove += 1
		widget.set_disk.remove(0)
	temp = []
	current_disk_num = -1
	for x in range(0, len(disk.disks)):
		disk()
		widget.set_disk.insert(x, str(x), disk.devices[x].path)
	widget.set_disk.set_active(get_sel)
def Nextmain(button):
	top_level = builder.main.get_object("top_level")
	builder.main_window.remove(top_level)
	toplevel_window = builder.ask_part.get_object("top_level")
	toplevel_window.reparent(builder.main_window)
	builder.main_window.add(toplevel_window)
def Nextask_part(button):
	top_level = builder.ask_part.get_object("top_level")
	builder.main_window.remove(top_level)
	if defaults.install_type == "custom":
		toplevel_window = builder.adv_part.get_object("top_level")
		toplevel_window.reparent(builder.main_window)
		builder.main_window.add(toplevel_window)
		do_part_first(0)
	else:
		toplevel_window = builder.rootPart.get_object("top_level")
		disk()
		widget.part_size.set_upper(disk.size[0]*1024)
		for x in range(0, len(disk.disks)):
			disk()
			widget.root_disk.insert(x, str(x), disk.device.path[x])
			widget.root_disk.set_active(0) 
		toplevel_window.reparent(builder.main_window)
		builder.main_window.add(toplevel_window)
def Backask_part(button):
	top_level = builder.ask_part.get_object("top_level")
	builder.main_window.remove(top_level)
	toplevel_window = builder.main.get_object("top_level")
	toplevel_window.reparent(builder.main_window)
	builder.main_window.add(toplevel_window)
def Nextadv_part(button):
	top_level = builder.adv_part.get_object("top_level")
	builder.main_window.remove(top_level)
	toplevel_window = builder.xserver.get_object("top_level")
	toplevel_window.reparent(builder.main_window)
	builder.main_window.add(toplevel_window)
def Backadv_part(button):
	top_level = builder.adv_part.get_object("top_level")
	builder.main_window.remove(top_level)
	toplevel_window = builder.ask_part.get_object("top_level")
	toplevel_window.reparent(builder.main_window)
	builder.main_window.add(toplevel_window)
def Backxserver(button):
	top_level = builder.xserver.get_object("top_level")
	builder.main_window.remove(top_level)
	toplevel_window = builder.adv_part.get_object("top_level")
	toplevel_window.reparent(builder.main_window)
	builder.main_window.add(toplevel_window)
def Nextvar(button):
	top_level = builder.var.get_object("top_level")
	builder.main_window.remove(top_level)
	toplevel_window = builder.user.get_object("top_level")
	toplevel_window.reparent(builder.main_window)
	widget.hostname_error_label.set_visible(False)
	widget.hostname_ok.set_visible(False)
	widget.username_error_label.set_visible(False)
	widget.username_ok.set_visible(False)
	widget.password_error_label.set_visible(False)
	widget.password_ok.set_visible(False)
	builder.main_window.add(toplevel_window)
def Backvar(button):
	top_level = builder.var.get_object("top_level")
	builder.main_window.remove(top_level)
	toplevel_window = builder.adv_part.get_object("top_level")
	toplevel_window.reparent(builder.main_window)
	builder.main_window.add(toplevel_window)
def Nextroot(button):
	top_level = builder.rootPart.get_object("top_level")
	builder.main_window.remove(top_level)
	toplevel_window = builder.user.get_object("top_level")
	toplevel_window.reparent(builder.main_window)
	widget.hostname_error_label.set_visible(False)
	widget.username_error_label.set_visible(False)
	widget.password_error_label.set_visible(False)
	builder.main_window.add(toplevel_window)
def Backroot(button):
	top_level = builder.rootPart.get_object("top_level")
	builder.main_window.remove(top_level)
	toplevel_window = builder.ask_part.get_object("top_level")
	toplevel_window.reparent(builder.main_window)
	builder.main_window.add(toplevel_window)
def Backuser(button):
	top_level = builder.user.get_object("top_level")
	builder.main_window.remove(top_level)
	if defaults.install_type == "custom":
		toplevel_window = builder.var.get_object("top_level")
	else:
		toplevel_window = builder.rootPart.get_object("top_level")
	toplevel_window.reparent(builder.main_window)
	builder.main_window.add(toplevel_window)
#All functions that handle signals from glade
#All the stepAsk_part functions
def state_update(button):
	defaults.update = button.get_active()
	print ("Install Type: %s Updates: %s" % (defaults.install_type, defaults.update))
def set_default_install(button):
	state = button.get_active()
	if state == True:
		defaults.install_type = "default"
		defaults.update = True
		widget.check_update.set_active(True)
		widget.check_update.set_sensitive(False)
		print ("Install Type: %s Updates: %s" % (defaults.install_type, defaults.update))
def set_custom_install(button):
	state = button.get_active()
	if state == True:
		global install_type
		defaults.install_type = "custom"
		widget.check_update.set_sensitive(True)
		print ("Install Type: %s Updates: %s" % (defaults.install_type, defaults.update))
#All the steproot (simple partition detection) functions
def change_disk_root(combo):
	tree_iter = combo.get_active_iter()
	if tree_iter != None:
		model = combo.get_model()
		current_disk, disk_num = model[tree_iter][:2]
		disk_num = int(disk_num)
		print("Disk: %s" % current_disk, disk_num)
		disk()
		print (disk.unit)
		global current_root_disk
		current_root_disk = disk_num
		systype.gendev = current_disk
		print (defaults.current_root_size_unit)
		if defaults.current_root_size_unit == "m":
			if disk.unit[disk_num] == "G":
				widget.part_size.set_upper(disk.size[disk_num]*1024)
			elif disk.unit[disk_num] == "T":
				widget.part_size.set_upper(disk.size[disk_num]*1024*1024)
		else:
			if disk.unit[disk_num] == "G":
				widget.part_size.set_upper(disk.size[disk_num])
			elif disk.unit[disk_num] == "T":
				widget.part_size.set_upper(disk.size[disk_num]*1024)
			elif disk.unit[disk_num] == "M":
				widget.part_size.set_upper(float(disk.size[disk_num]/1024))
		widget.part_size.set_value(0)
def unit_change_root(combo):
	tree_iter = combo.get_active_iter()
	if tree_iter != None:
		model = combo.get_model()
		unit, id_num = model[tree_iter][:2]
		id_num = int(id_num)
		print("Unit: %s" % unit, id_num)
		if id_num == 1:
			disk()
			widget.part_size.set_upper(disk.size[current_root_disk]*1024)
			defaults.current_root_size_unit = "m"
		else:
			disk()
			widget.part_size.set_upper(disk.size[current_root_disk])
			defaults.current_root_size_unit = "g"
		widget.part_size.set_value(0)
def setRoot(adjustment):
	systype.rootSize = int(round(adjustment.get_value(), 0))
	print ("Root Partition Size: %s" % systype.rootSize)
#All the partAdv (Advanced partition detection) functions
def part_selected(selection):
	global treeiter
	model, treeiter = selection.get_selected()
	if treeiter != None:
		if model[treeiter][0] != "Free Space":
			widget.add_part.set_sensitive(False)
			widget.remove_part.set_sensitive(True)
			widget.change_part.set_sensitive(True)
			widget.format_iter.set_sensitive(True)
		else:
			widget.format_iter.set_sensitive(False)
			widget.add_part.set_sensitive(True)
			widget.remove_part.set_sensitive(False)
			widget.change_part.set_sensitive(False)
		global main_selection
		main_selection = str(model[treeiter][0])
		print ("Selected Part:", main_selection)
		current.size = str(widget.partitions.get_value(treeiter, 3))
		current.start = str(widget.partitions.get_value(treeiter, 5))
		current.end = str(widget.partitions.get_value(treeiter, 6))
		print ("Size:", current.size, "\n", "Start:", current.start, "\n", "End:", current.end)
def set_gentoo_device(combo):
	tree_iter = combo.get_active_iter()
	if tree_iter != None:
		model = combo.get_model()
		disk, id_num = model[tree_iter][:2]
		id_num = int(id_num)
		print("Disk: %s" % disk, id_num)
		global main_disk
		global main_disk_num
		main_disk_num = id_num
		main_disk = disk[:8]
		systype.gendev = main_disk
		builder.disk_type = diskType(main_disk)
		get_newpart()
		print (id_num)
		do_part(id_num)
		widget.add_part.set_sensitive(False)
		widget.change_part.set_sensitive(False)
		widget.remove_part.set_sensitive(False)
		widget.main_unit.set_active(1)
def set_grub_device(combo):
	tree_iter = combo.get_active_iter()
	if tree_iter != None:
		model = combo.get_model()
		grub_partiton, part_num = model[tree_iter][:2]
		part_num = str(part_num)
		print("Grub Partition: %s" % grub_partiton, part_num)
def do_part_button(button):
	num = widget.set_disk.get_active()
	do_part(num)
	do_disk()
def newpart(button):
	#Reset the values in newpart window
	widget.part_label.set_text("")
	widget.mount_point1.set_text("")
	widget.mount_point2.set_text("")
	widget.partition_edit_format_checkbutton.set_active(False)
	widget.partition_edit_format_checkbutton1.set_active(False)
	widget.format_type.set_active(-1)
	widget.format_type1.set_active(-1)
	widget.mount_point1.set_text("")
	widget.mount_point2.set_text("")
	num = widget.set_disk.get_active()
	#Get the values of the selected free space
	start = float(widget.partitions.get_value(treeiter, 5)[:-3]) + 1
	end = float(widget.partitions.get_value(treeiter, 6)[:-3]) - 1
	size = float(widget.partitions.get_value(treeiter, 3)[:-3]) - 1
	type = widget.partitions.get_value(treeiter, 2)
	
	#Set the radio button value for logical/primary
	if type == "free_logical":
		widget.logical.set_active(True)
		widget.primary.set_active(False)
		widget.primary.set_sensitive(False)
		widget.logical.set_label("Logical")
	else:
		widget.logical.set_active(False)
		widget.primary.set_active(True)
		widget.primary.set_sensitive(True)
		if "extended" in part.type:
			widget.logical.set_label("Logical")
		else:
			widget.logical.set_label("Extended")
	
	#Set the upper for adjustments
	widget.part_size_adj.set_upper(size)
	widget.start_part.set_upper(end)
	widget.end_part.set_upper(end)
	#Set the lower for adjustments
	widget.start_part.set_lower(start)
	widget.end_part.set_lower(start)
	#Set Value for adjustments
	widget.part_size_adj.set_value(size)
	widget.start_part.set_value(start)
	widget.end_part.set_value(end)
	
	#Display the new window
	builder.disk_type = diskType(main_disk)
	get_newpart()
	builder.dialog_new_part.show_all()
def remove_part(button):
	builder.are_you_sure.show_all()
def editpart(button):
	widget.format_type_change.set_active(-1)
	widget.mount_point_change.set_text("")
	builder.edit_part.show_all()
def mk_table(button):
	widget.table_type.set_active(-1)
	builder.make_table.show_all()
def close(button):
	builder.dialog_new_part.hide()
def close_sure(button):
	builder.are_you_sure.hide()
def close_table(button):
	builder.make_table.hide()
def close_change(button):
	builder.edit_part.hide()
def part_format_type(combo):
	tree_iter = combo.get_active_iter()
	if tree_iter != None:
		model = combo.get_model()
		string, current.fileSystem = model[tree_iter][:2]
		print("Format Type: %s" % string, current.fileSystem)
def format_part(button):
	systype.fs_id = widget.format_type_change.get_active()
	systype.fs_id += 1
	print (systype.fs_id)
	umount = os.system("umount %s" % main_selection)
	if umount != 0:
		print ("Continueing")
	if systype.fs_id != 3:
		print (systype.fs_id)
		if systype.fs_id == 1:
			print ("Formating in ext2")
			os.system("mkfs.ext2 -F -T small %s" % (main_selection))
		elif systype.fs_id == 2:
			print ("Formating in ext4")
			os.system("mkfs.ext4 -F -T small %s" % (main_selection))
		elif systype.fs_id == 4:
			if current.size > 2:
				if current.unit == "G":
					print ("Formating in vfat")
					os.system("mkfs.vfat %s" % (main_selection))
				elif current.unit == "M":
					if current.size > 2048:
						print ("Formating in vfat")
						os.system("mkfs.vfat %s" % (main_selection))
					else:
						print ("Formating in fat16")
						os.system("mkdosfs -F 16 %s" % (main_selection))
				elif current.unit == "K":
					if current.size > 2048*1024:
						print ("Formating in vfat")
						os.system("mkfs.vfat %s" % (main_selection))
					else:
						print ("Formating in fat16")
						os.system("mkdosfs -F 16 %s" % (main_selection))
				else:
					print ("Formating in vfat")
					os.system("mkfs.vfat %s" % (main_selection))
	elif systype.fs_id == 3:
		print ("Formating in ntfs")
		os.system("mkntfs -F %s" % main_selection)
	mount_point = widget.mount_point_change
	try:
		a = mount_point[0]
	except TypeError:
		mount_point = widget.mount_point_change.get_text()
	if mount_point != "" or mount_point != " ":
		try:
			if mount_point[0] == "/":
				mkdir = os.system("find %s" % mount_point)
				if mkdir != 0 :
					os.system("mkdir %s" % mount_point)
				os.system("mount %s %s" % (main_selection, mount_point))
		except IndexError:
			pass
	num = widget.set_disk.get_active()
	do_part(num)
	builder.edit_part.hide()
	widget.add_part.set_sensitive(False)
	widget.change_part.set_sensitive(False)
	widget.remove_part.set_sensitive(False)
def remove_sel_part(button):
	os.system("umount %s%s" % (main_disk, int(main_selection[8:])))
	os.system("parted -s %s rm %s" % (main_disk, int(main_selection[8:])))
	builder.are_you_sure.hide()
	num = widget.set_disk.get_active()
	do_part(num)
	widget.add_part.set_sensitive(False)
	widget.change_part.set_sensitive(False)
	widget.remove_part.set_sensitive(False)
def mount_point_change(entry):
	widget.mount_point_change.set_text(entry.get_text())
def set_primary(button):
	if button.get_active():
		state = "on"
		current.primary = True
	else:
		current.primary = False
		state = "off"
	print("Primary:", state)
	print ("current.primary = %s" % current.primary)
def set_logical(button):
	if button.get_active():
		state = "on"
		current.primary = False
	else:
		current.primary = True
		state = "off"
	print("Logical:", state)
	print ("current.primary = %s" % current.primary)
def size_in(*arg):
	size = arg[0].get_value()
	print ("Size:", size)
	start = widget.start_part.get_value()
	widget.end_part.set_value(start+size)
def start_in(*arg):
	size = arg[0].get_value()
	print ("Start:", size)
	start = widget.start_part.get_value()
	end = widget.end_part.get_value()
	widget.part_size_adj.set_value(end-start)
def end_in(*arg):
	size = arg[0].get_value()
	print ("End:", size)
	end = widget.end_part.get_value()
	start = widget.start_part.get_value()
	widget.part_size_adj.set_value(end-start)
def part_format_type(combo):
	tree_iter = combo.get_active_iter()
	if tree_iter != None:
		model = combo.get_model()
		current.fstype, current.fileSystem = model[tree_iter][:2]
		print("Format Type: %s" % current.fstype, current.fileSystem)
def state_format(button):
	if button.get_active():
		state = "on"
		print ("Formating Partition")
		current.formatyn = True
	else:
		state = "off"
		print ("Not Formating Partition")
		current.formatyn = False
def mount_point(entry):
	current.mount_point = entry.get_text()
	print (current.mount_point)
def get_start(size):
	end = current.end
	end_unit = end[-3]
	end = end.replace("iB", "")
	end = end.replace(end_unit, "")
	end = float(end)
	start = end-size
	return start
def get_end(size):
	start = current.start
	start_unit = start[-3]
	start = start.replace("iB", "")
	start = start.replace(start_unit, "")
	start = float(start)
	end = start+size
	return end
def part_label(entry):
	current.label = entry.get_text()
	print (current.label)
def find_table(combo):
	tree_iter = combo.get_active_iter()
	if tree_iter != None:
		global table_id
		global part_table
		model = combo.get_model()
		part_table, table_id = model[tree_iter][:2]
		table_id = int(table_id)
		print("Partition Table: %s" % part_table, table_id)
def make_table(button):
	if table_id == 1:
		table = "gpt"
	else:
		table = "ms"
	os.system("parted -s %s mktable %s" % (main_disk, table))
	num = widget.set_disk.get_active()
	do_part(num)
	builder.make_table.hide()
def convert_unit(input_unit, output_unit):
	input_id = 0
	output_id = 0
	if input_unit == "K":
		input_id = 1
	elif input_unit == "M":
		input_id = 2
	elif input_unit == "G":
		input_id = 3
	if output_unit == "K":
		output_id = 1
	elif output_unit == "M":
		output_id = 2
	elif output_unit == "G":
		output_id = 3
	if output_id == input_id:
		print ("Unit does not need converting!")
		return input_unit
def mk_part(button):
	disk_type = diskType(main_disk)
	arg1 = main_disk
	arg4 = int(widget.start_part.get_value())
	arg5 = int(widget.end_part.get_value())
	size = widget.part_size_adj.get_value()
	if disk_type != "gpt":
		if current.primary:
			arg2 = "primary"
		else:
			arg2 = widget.logical.get_label()
	else:
		arg2 = current.label
	if arg2 == "Extended":
		arg2 = "extended"
		arg3 = ""
	else:
		arg3 = current.fileSystem
	if arg3 == "fat":
		if arg5 - arg4 > 2048:
			arg3 = "fat32"
		else:
			arg3 = "fat16"
	old = part.paths
	os.system("parted -s %s unit MiB mkpart %s %s %s %s" % (arg1, arg2, arg3, arg4, arg5))
	do_part(disk.disks.index(main_disk))
	new = part.paths
	path = get_difference(old, new)[0]
	if widget.partition_edit_format_checkbutton.get_active() or widget.partition_edit_format_checkbutton1.get_active():
		fileSystemWrite(path, arg3)
	try:
		current.mount_point[0]
	except IndexError:
		pass
	else:
		if current.mount_point[0] == "/":
			if not os.path.exists("%s" % current.mount_point):
				os.system("mkdir %s" % current.mount_point)
			os.system("mount %s %s" % (path, current.mount_point))
	do_part(disk.disks.index(main_disk))
	builder.dialog_new_part.hide()
def fileSystemWrite(path, fstype):
	programs = {
		"linux-swap(v1)": "mkswap",
		"ext2": "mkfs.ext2 -T small -F",
		"ext3": "mkfs.ext3 -T small -F",
		"ext4": "mkfs.ext4 -T small -F",
		"fat32": "mkfs.vfat -F32",
		"fat16": "mkfs.vfat -F16",
		"ntfs": "mkfs.ntfs -F -Q"}
	os.system("%s %s" % (programs[fstype], path))
def format_iter(*args):
	path = widget.partitions.get_value(treeiter, 0)
	widget.recalculating.set_label("Formating partition %s" % path)
	widget.recalculating.set_visible(True)
	builder.main_window.show_all()
	fstype = widget.partitions.get_value(treeiter, 1)
	builder.main_window.show_all
	fileSystemWrite(path, fstype)
	widget.recalculating.set_visible(False)
#All the stepVar functions (advanced gentoo settings)
def desktop_environment(combo):
	tree_iter = combo.get_active_iter()
	if tree_iter != None:
		model = combo.get_model()
		systype.profile_sel, systype.profile_num = model[tree_iter][:2]
		systype.profile_num = int(systype.profile_num)
		print("desktop_environment: %s" % systype.profile_sel, systype.profile_num)
		systype.memtotal = subprocess.check_output("cat /proc/meminfo | grep MemTotal", shell=True)
		systype.memtotal = systype.memtotal[9:]
		systype.memtotal = systype.memtotal[:-3]
		systype.memtotal = systype.memtotal.replace(" ", "")
		systype.memtotal = int(systype.memtotal)
		systype.memtotal /= 1024
		systype.memtotal = int(round(systype.memtotal, 0))
		widget.memory_bar.set_min_value(0)
		widget.memory_bar.set_max_value(systype.memtotal)
		if systype.profile_num == 1:
			systype.usedmem = 110
			makeopts.use_flags = "X -gnome -kde gtk gtk3 qt qt5 sse sse2"
		elif systype.profile_num == 2:
			systype.usedmem = 245
			makeopts.use_flags = "X gnome -kde gtk gtk3 qt -qt5 sse sse2"
		elif systype.profile_num == 3:
			systype.usedmem = 303
			makeopts.use_flags = "X -gnome kde gtk gtk3 qt qt5 sse sse2"
		elif systype.profile_num == 4:
			systype.usedmem = 354
			makeopts.use_flags = "X -gnome kde gtk gtk3 qt qt5 sse sse2"
		elif systype.profile_num == 5:
			systype.usedmem = 0
			makeopts.use_flags = "sse sse2"
		widget.memory_bar.set_max_value(systype.memtotal)
		widget.memory_bar.set_value(systype.usedmem)
		umem = float(systype.usedmem)
		tmem = float(systype.memtotal)
		mempercent = int(round((umem/tmem)*100, 0))
		widget.memory_label.set_markup("""<small>Total Memory
	<span color="grey">%s %s</span></small>""" % (mempercent, "%"))
		if systype.profile_num == 1:
			widget.desktop_manager.remove(0)
			widget.desktop_manager.remove(0)
			widget.dm_state.set_sensitive(True)
			widget.desktop_manager.insert(0, "ldm", "Lightdm")
			widget.desktop_manager.insert(1, "xdm", "XDM (X-Org)")
			widget.dm_state.set_active(True)
			widget.desktop_manager.set_sensitive(True)
		elif systype.profile_num == 2:
			widget.desktop_manager.remove(0)
			widget.desktop_manager.remove(0)
			widget.dm_state.set_sensitive(True)
			widget.desktop_manager.insert(0, "ldm", "Lightdm")
			widget.desktop_manager.insert(1, "gdm", "GDM GNOME Desktop Manager")
			widget.dm_state.set_active(True)
			widget.desktop_manager.set_sensitive(True)
		elif systype.profile_num == 3 or systype.profile_num == 4:
			widget.desktop_manager.remove(0)
			widget.desktop_manager.remove(0)
			widget.dm_state.set_sensitive(True)
			widget.desktop_manager.insert(0, "ldm", "Lightdm")
			widget.desktop_manager.insert(1, "kdm", "KDM K Desktop Manager")
			widget.dm_state.set_active(True)
			widget.desktop_manager.set_sensitive(True)
		else:
			widget.dm_state.set_active(False)
			widget.dm_state.set_sensitive(False)
			widget.desktop_manager.set_sensitive(False)
def dm_state(switch, gparam):
	if switch.get_active():
		state = "on"
	else:
		state = "off"
	systype.dm_state = state
	if systype.dm_state == "on":
		widget.desktop_manager.set_sensitive(True)
	else:
		widget.desktop_manager.set_sensitive(False)
	print("Desktop Manager State:", state)
def desktop_manager(combo):
	tree_iter = combo.get_active_iter()
	if tree_iter != None:
		model = combo.get_model()
		systype.dm, systype.dm_id = model[tree_iter][:2]
		try:
			systype.dm_id = int(systype.dm_id)
		except ValueError:
			systype.dm_id = str(systype.dm_id)
		print("Desktop Manager: %s" % systype.dm, systype.dm_id)
def install_de_install(button):
	systype.de_ins = button.get_active()
	print ("Install Desktop Manager: %s" % systype.de_ins)
def install_custom_packages(button):
	systype.ins_custom = widget.enable_custom_packages.get_active()
	widget.add_packages.set_sensitive(button.get_active())
	print ("Install Custom Packages: %s" % systype.ins_custom)
def add_package(button):
	widget.scrollable_treelist.set_vexpand(True)
	widget.top_level_pkg.pack_start(widget.scrollable_treelist_pkg, expand=True, fill=True, padding=0)
	widget.scrollable_treelist_pkg.add(widget.treeview_pkg)
	widget.scrollable_treelist_pkg.set_size_request(250, 150)
	widget.scrollable_treelist_pkg.show_all()
	builder.add_package_win.show_all()
def close_packages(button):
	builder.add_package_win.hide()
def package_select(selection):
	global treeiter_pkg
	model, treeiter_pkg = selection.get_selected()
	if treeiter_pkg != None:
		widget.delete_pkg.set_sensitive(True)
		global pkg_sel
		pkg_sel = str(model[treeiter_pkg][0])
		print ("Selected Package:", pkg_sel)
	else:
		widget.delete_pkg.set_sensitive(False)
def rm_pkg(button):
	widget.packages.remove(treeiter_pkg)
def add_pkg(button):
	widget.pkg_temp_str = widget.pkg_name.get_text()
	widget.pkg_name.set_text("")
	widget.new_package_win.show_all()
def pkg_name(entry):
	widget.pkg_temp_str = entry.get_text()
	print ("Current Package: %s" % widget.pkg_temp_str)
def add_package_cfm(button):
	package.list.append(widget.pkg_temp_str)
	widget.packages.append([widget.pkg_temp_str])
	widget.new_package_win.hide()
	print ("Added Package: %s" % widget.pkg_temp_str)
def close_new_pkg(button):
	widget.new_package_win.hide()
def root_passwd(entry):
	defaults.root_passwd = entry.get_text()
	if len(defaults.root_passwd) > 0 and len(defaults.root_passwd) < 5:
		widget.root_strength.set_markup("""<small><span foreground="darkred">Weak Password</span></small>""")
	elif len(defaults.root_passwd) >= 5 and len(defaults.root_passwd) < 10:
		widget.root_strength.set_markup("""<small><span foreground="#C3B74D">Medium Password</span></small>""")
	elif len(defaults.root_passwd) >= 10:
		widget.root_strength.set_markup("""<small><span foreground="green">Strong Password</span></small>""")
	if len(defaults.root_passwd) != 0:
		widget.root_strength.set_visible(True)
	else:
		widget.root_strength.set_visible(False)
def change_unit_adv(combo):
	tree_iter = combo.get_active_iter()
	if tree_iter != None:
		global main_unit
		model = combo.get_model()
		main_unit, id_num = model[tree_iter][:2]
		id_num = str(id_num)
		do_part(main_disk_num, id_num) 
		print("Selected Unit: %s" % main_unit, id_num)
def change_nvidia(combo):
	tree_iter = combo.get_active_iter()
	if tree_iter != None:
		model = combo.get_model()
		full_name, gpu.driver_version = model[tree_iter][:2]
		gpu.driver_version = str(gpu.driver_version)
		print("Selected Driver: %s" % full_name, gpu.driver_version)
def driver_warning(combo):
	tree_iter = combo.get_active_iter()
	if tree_iter != None:
		model = combo.get_model()
		full_name, gpu.driver_version = model[tree_iter][:2]
		gpu.driver_version = str(gpu.driver_version)
		if gpu.driver_version == gpu.working_driver_version:
			return
	if defaults.driver_cpu_selected == 0:
		widget.driver_warning.show_all()
	else:
		defaults.driver_cpu_selected = 0
def driver_decline(button):
	defaults.driver_cpu_selected == 1
	widget.gpu_version_change_nvidia.set_active(gpu.version_list.index(gpu.working_driver_version))
	widget.driver_warning.hide()
def driver_accept(button):
	widget.driver_warning.hide()
def optimize_use(button):
	defaults.optimize = button.get_active()
	print ("Optimize: %s" % defaults.optimize)
	if button.get_active() == True:
		makeopts.cflags = "-march=native -O2 -pipe"
	else:
		makeopts.cflags = ""
	widget.optimize_warning.set_visible(button.get_active())
	widget.custom_cflags_button.set_visible(button.get_active())
def open_cflags(button):
	widget.custom_cflags_buffer.set_text(makeopts.cflags)
	widget.cflags_window.show()
def save_cflags(button):
	start_iter = widget.custom_cflags_buffer.get_start_iter()
	end_iter = widget.custom_cflags_buffer.get_end_iter()
	makeopts.cflags = widget.custom_cflags_buffer.get_text(start_iter, end_iter, True)
	widget.cflags_window.hide()
def quit_cflags(*arg):
	widget.cflags_window.hide()
def cflag_help(button):
	os.system("google-chrome-stable https://wiki.gentoo.org/wiki/Safe_CFLAGS")
#All the stepUser functions (account and hostname setting)
def hostname(entry):
	defaults.hostname = entry.get_text()
	if len(defaults.hostname) != 0:
		widget.hostname_ok.set_visible(True)
		widget.hostname_error_label.set_visible(False)
	else:
		widget.hostname_ok.set_visible(False)
def username(entry):
	defaults.username = entry.get_text()
	if len(defaults.username) != 0:
		widget.username_ok.set_visible(True)
		widget.username_error_label.set_visible(False)
	else:
		widget.username_ok.set_visible(False)
def user_passwd(entry):
	defaults.user_passwd = entry.get_text()
	if len(defaults.user_passwd) > 0 and len(defaults.user_passwd) < 5:
		widget.password_strength.set_markup("""<small><span foreground="darkred">Weak Password</span></small>""")
	elif len(defaults.user_passwd) >= 5 and len(defaults.user_passwd) < 10:
		widget.password_strength.set_markup("""<small><span foreground="#C3B74D">Medium Password</span></small>""")
	elif len(defaults.user_passwd) >= 10:
		widget.password_strength.set_markup("""<small><span foreground="green">Strong Password</span></small>""")
	if len(defaults.user_passwd) != 0:
		widget.password_strength.set_visible(True)
	else:
		widget.password_strength.set_visible(False)
def user_passwd_cfm(entry):
	defaults.user_passwd_cfm = entry.get_text()
	if defaults.user_passwd_cfm == defaults.user_passwd:
		widget.password_ok.set_visible(True)
		widget.password_error_label.set_visible(False)
	else:
		widget.password_ok.set_visible(False)
def do_install(button):
	do_return = 0
	if len(defaults.root_passwd) == 0:
		widget.root_strength.set_visible(True)
		widget.root_strength.set_markup("""<small><span foreground="darkred">You must enter a password</span></small>""")
		do_return = 1
	if len(defaults.hostname) == 0:
		widget.hostname_error_label.set_visible(True)
		do_return = 1
	if len(defaults.user_passwd) == 0:
		widget.password_strength.set_visible(True)
		widget.password_strength.set_markup("""<small><span foreground="darkred">You must enter a password</span></small>""")
		do_return = 1
	if len(defaults.username) == 0:
		widget.username_error_label.set_visible(True)
		do_return = 1
	if len(defaults.user_passwd_cfm) == 0:
		widget.password_error_label.set_visible(True)
		do_return = 1
	if do_return == 1:
		return
	print ("Installing System...")
	top_level = builder.user.get_object("top_level")
	builder.main_window.remove(top_level)
	toplevel_window = builder.install.get_object("top_level")
	toplevel_window.reparent(builder.main_window)
	if defaults.install_type == "custom":
		widget.install_info.set_text("Checking for mounted devices...")
	else:
		widget.install_info.set_text("Mounting Required block devices...")
	builder.main_window.add(toplevel_window)
#Misc functions
def show_terminal(button):
	if button.get_active():
		widget.terminal_window.show_all()
	else:
		widget.terminal_window.hide()
def shutdown(*args):
	Gtk.main_quit()
def goto_xfce(button):
	xserver.xserver = "xfce"
	print (xserver.xserver)
	top_level = builder.xserver.get_object("top_level")
	builder.main_window.remove(top_level)
	toplevel_window = builder.xfce.get_object("top_level")
	toplevel_window.reparent(builder.main_window)
	builder.main_window.add(toplevel_window)
def goto_gnome(button):
	xserver.xserver = "gnome"
	print (xserver.xserver)
	top_level = builder.xserver.get_object("top_level")
	builder.main_window.remove(top_level)
	toplevel_window = builder.gnome.get_object("top_level")
	toplevel_window.reparent(builder.main_window)
	builder.main_window.add(toplevel_window)
def goto_kde(button):
	xserver.xserver = "kde"
	print (xserver.xserver)
	top_level = builder.xserver.get_object("top_level")
	builder.main_window.remove(top_level)
	toplevel_window = builder.kde.get_object("top_level")
	toplevel_window.reparent(builder.main_window)
	builder.main_window.add(toplevel_window)
def goto_none(button):
	xserver.xserver = "none"
	print (xserver.xserver)
	top_level = builder.xserver.get_object("top_level")
	builder.main_window.remove(top_level)
	toplevel_window = builder.user.get_object("top_level")
	toplevel_window.reparent(builder.main_window)
	builder.main_window.add(toplevel_window)
def dm_next ( gui ):
	exec ( "top_level = builder.%s.get_object(\"top_level\")" % gui )
	builder.main_window.remove(top_level)
	toplevel_window = builder.binhost.get_object("top_level")
	toplevel_window.reparent(builder.main_window)
	builder.main_window.add(toplevel_window)
def xdm(button):
	xserver.display_manager = "xdm"
	dm_next ( "xfce" )
def gdm(button):
	xserver.display_manager = "gdm"
	dm_next ( "gnome" )
def kdm(button):
	xserver.display_manager = "kdm"
	dm_next ( "kde" )
def lightdm(button):
	xserver.display_manager = "lightdm"
	dm_next ( xserver.xserver )
def startx(button):
	xserver.display_manager = "startx"
	dm_next ( xserver.xserver )

main_handlers = {
	"exit": Gtk.main_quit,
	"shutdown": shutdown,
	"Next": Nextmain
	}
part_ask_handlers = {
	"default_install": set_default_install,
	"custom_install": set_custom_install,
	"state_updates": state_update,
	"Back": Backask_part,
	"Next": Nextask_part,
	}
part_advanced_handlers = {
	"new_part": newpart,
	"remove_part": remove_part,
	"edit_part": editpart,
	"new_tbl": mk_table,
	"reload_part": do_part_button,
	"gentoo_device": set_gentoo_device,
	"Next": Nextadv_part,
	"Back": Backadv_part,
	"grub_device": set_grub_device,
	"change_unit_adv": change_unit_adv,
	"cancel": close,
	"close_sure": close_sure,
	"close_table": close_table,
	"close_change": close_change,
	"part_format_type": part_format_type,
	"format_part": format_part,
	"remove_yes": remove_sel_part,
	"mount_point_change": mount_point_change,
	"set_primary": set_primary,
	"set_logical": set_logical,
	"size_in": size_in,
	"start_in": start_in,
	"end_in": end_in,
	"format_type": part_format_type,
	"state_format": state_format,
	"mount_point": mount_point,
	"part_label": part_label,
	"mk_part": mk_part,
	"new_table": mk_table,
	"make_table": make_table,
	"find_table": find_table,
	"format_iter": format_iter
	}
var_handlers = {
	"Next": Nextvar,
	"Back": Backvar,
	"desktop_environment": desktop_environment,
	"dm_state": dm_state,
	"desktop_manager": desktop_manager,
	"install_de_install": install_de_install,
	"ins_custom": install_custom_packages,
	"add_package": add_package,
	"close_packages": close_packages,
	"rm_pkg": rm_pkg,
	"pkg_name": pkg_name,
	"add_package_cfm": add_package_cfm,
	"close_new_pkg": close_new_pkg,
	"add_pkg_main": add_pkg,
	"change_nvidia": change_nvidia,
	"driver_warning": driver_warning,
	"driver_decline": driver_decline,
	"driver_accept": driver_accept,
	"optimize_use": optimize_use,
	"save_cflags": save_cflags,
	"quit_cflags": quit_cflags,
	"open_cflags": open_cflags,
	"cflag_help": cflag_help,
	}
xserver_handlers = {
	"xfce": goto_xfce,
	"gnome": goto_gnome,
	"kde": goto_kde,
	"none": goto_none,
	"Back": Backxserver}
xfce_handlers = {
	"xdm": xdm,
	"lightdm" : lightdm,
	"startx" : startx
	}
gnome_handlers = {
	"gdm": gdm,
	"lightdm" : lightdm,
	"startx" : startx
	}
kde_handlers = {
	"kdm": kdm,
	"lightdm" : lightdm,
	"startx" : startx
	}
rootPart_handlers = {
	"Next": Nextroot,
	"Back": Backroot,
	"change_disk" : change_disk_root,
	"root_size_unit": unit_change_root,
	"setRoot": setRoot
	}
user_handlers = {
	"Next": Gtk.main_quit,
	"Back": Backuser,
	"root_passwd": root_passwd,
	"hostname": hostname,
	"username": username,
	"user_passwd": user_passwd,
	"user_passwd_cfm": user_passwd_cfm,
	"install": do_install,
	}
install_handlers = {
	"show_terminal": show_terminal,
	}
class main():
	def __init__(self):
		widget.select.connect("changed", part_selected)
		widget.select_pkg.connect("changed", package_select)
		builder.main.connect_signals(main_handlers)
		builder.ask_part.connect_signals(part_ask_handlers)
		builder.xserver.connect_signals(xserver_handlers)
		builder.gnome.connect_signals(gnome_handlers)
		builder.kde.connect_signals(kde_handlers)
		builder.xfce.connect_signals(xfce_handlers)
		builder.rootPart.connect_signals(rootPart_handlers)
		builder.adv_part.connect_signals(part_advanced_handlers)
		builder.var.connect_signals(var_handlers)
		builder.user.connect_signals(user_handlers)
		builder.install.connect_signals(install_handlers)
		builder.main_window.show_all()
		Gtk.main()
