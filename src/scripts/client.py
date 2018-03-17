#!/usr/bin/python

from op_socket import Address
from interface import Server, Host
import readline
from log import Log
import os

ANSI_BOLD = "\x1b[1m"
ANSI_GREEN = "\x1b[32m"
ANSI_RESET = "\x1b[0m"
native_cflags = os.popen("gcc -### -E - -march=native 2>&1 | sed -r '/cc1/!d;s/(\")|(^.* - )//g'").read()[:-1]


class CommandManager:
	"""
	The client interface to the server daemon

	"""
	commands = {}
	help_size = 0
	help_text = ""
	keep_going = True
	name = ""
	
	def __init__(self, name, help_size=20, help_text=""):
		self.name = name
		self.help_size = help_size
		self.help_text = help_text
		self.commands = {}
		self.keep_going = True
	
	def new_command(self, cmd):
		self.commands[cmd.selector] = cmd
	
	def help(self):
		print("%sCommands:" % (self.name + ("\n" + self.help_text + "\n" + "\n") if len(self.help_text) > 0 else ""))
		for key in self.commands:
			self.commands[key].print_help()
	
	def commandline(self):
		while self.keep_going:
			k = input("%s%sautogentoo> %s" % (ANSI_GREEN, ANSI_BOLD, ANSI_RESET)).split(" ")
			if not k[0]:
				continue
			try:
				self.commands[k[0]]
			except KeyError:
				print("command '%s' not found" % k[0])
				continue
			target_cmd = self.commands[k[0]]
			if len(k[1:]) != target_cmd.argc:
				print("'%s' requires %s arguments got %s" % (k[0], target_cmd.argc, len(k[1:])))
				continue
			target_cmd.run(k[1:])


class Command:
	selector = ""
	argc = 0
	argv = []
	help = ""
	manager = None
	fptr = None
	
	def __init__(self, manager, selector, func, argv=None, _help=""):
		self.selector = selector
		if argv is None:
			self.argc = 0
		else:
			self.argc = len(argv)
		self.argv = argv
		self.help = _help
		self.manager = manager
		self.fptr = func
	
	def print_help(self):
		if self.argv is None:
			self.argv = []
		ar_list = ', '.join(self.argv)
		print("    %s%s%s %s" % (
			self.selector, " " + ar_list, ' ' * (self.manager.help_size - (len(self.selector) + len(ar_list))), self.help))
	
	def run(self, args):
		self.fptr(*args)


def limit_width(__str, indent, size=60):
	out = ""
	while len(__str) > 0:
		buf = min(size, len(__str))
		out += __str[:buf] + "\n" + " " * indent
		__str = __str[buf:]
	return out[:-1 * (indent + 1)]


def find_host(server: Server, host_id: str) -> [Host, None]:
	for host in server.hosts:
		if host_id == host.get("id"):
			return host
	return None


def print_hosts(server: Server):
	for host in server.hosts:
		print("%s: %s" % (host.get("id"), host.get("hostname")))


def print_host(host: Host):
	print("%s\n%s\nprofile=%s\nchost=%s\narch=%s\nCFLAGS=%s\nUSE=%s\n" % (
		host.get("id"),
		host.get("hostname"),
		host.get("profile"),
		host.get("chost"),
		host.get("arch"),
		limit_width(host.get("cflags"), 7),
		limit_width(host.get("use"), 4, 63))
		)


def rlinput(prompt, prefill=''):
	readline.set_startup_hook(lambda: readline.insert_text(prefill))
	try:
		return input(prompt)
	finally:
		readline.set_startup_hook()


def edit_host(host: Host):
	values = []
	extra = host.get_extra()
	for i, x in enumerate(["hostname", "profile", "cflags", "use"]):
		values.append(host.get(x))
		print("[%d] %s = %s" % (i + 1, x, host.get(x)))
	print("[5] extra = %s" % extra)
	
	k = ""
	f1 = int(input("Field to edit > "))
	f2 = -1
	last = f1
	if f1 == 5:
		for i, x in enumerate(extra):
			print("[%s] %s" % (i + 1, x))
		f2 = int(input("Field to edit > "))
		values = extra
		last = f2
	else:
		k = ["hostname: ", "profile: ", "cflags (-march=native is auto replaced): ", "use: "][f1 - 1]
	
	new_val = rlinput(k, values[last - 1])
	if k == "cflags: ":
		new_val.replace("-march=native", native_cflags)
	host.set_field(f1 - 1, f2 - 1, new_val)


def new_host(server: Server):
	print("Choose a template for new environ, or new")
	for x in server.templates:
		print("%s (%s)" % (x.get("id"), x.get("arch")))
	
	template = None
	templ = input("template > ")
	if templ == "new":
		while 1:
			arch = input("portage arch (amd64/x86/arm) > ")
			spec_arch = arch
			if arch in ("arm", "x86"):
				spec_arch = input("actual arch (i686, armv7a) > ")
			feature = input("option feature (systemd/ulibc/hardened) > ")
			if len(feature) > 0:
				feature = '-' + feature
			import http.client
			c = http.client.HTTPConnection("distfiles.gentoo.org")
			c.request("HEAD", "/releases/%s/autobuilds/latest-stage3-%s.txt" % (arch, spec_arch + feature))
			if c.getresponse().status != 200:
				Log.error("template id '%s' or arch '%s' is invalid\n", spec_arch + feature, arch)
				Log.error(
					"http://distfiles.gentoo.org/releases/%s/autobuilds/latest-stage3-%s.txt does not exist\n",
					arch, spec_arch + feature)
				continue
			break
		
		chost = rlinput("chost > ", "x86_64-pc-linux-gnu")
		cflags = rlinput("cflags (-march=native is auto converted) > ", "-march=native -O2 -pipe")
		cflags = cflags.replace("-march=native", native_cflags)
		
		make_extra = []
		me_buff = input("append to make.conf > ")
		while len(me_buff) > 0:
			make_extra.append(me_buff)
			me_buff = input("append to make.conf > ")
		
		templ = server.new_template(spec_arch + feature, arch, chost, cflags, make_extra)
	
	for t in server.templates:
		if t.get("id") == templ:
			template = t
			break
	if template is None:
		Log.error("template '%s' could not be found\n" % templ)
	
	server.new_host([
		input("hostname > "),
		rlinput("profile > ", "default/linux/amd64/17.0/desktop/gnome/systemd"),
		rlinput("chost > ", "x86_64-pc-linux-gnu"),
		input("cflags > "),
		rlinput("use > ", "mmx sse sse2 systemd")
	])


def main():
	server = Server(Address("localhost", 9490))
	server.read_server()
	
	cmdline = CommandManager("AutoGentoo CLI", help_text="The autogentoo user interface")
	commands = [
		Command(cmdline, "refresh", lambda: server.read_server(), _help="refresh the server data"),
		Command(cmdline, "help", lambda: cmdline.help(), _help="Print the help page"),
		Command(cmdline, "list", lambda: print_hosts(server), _help="List all the available hosts"),
		Command(
			cmdline,
			"host",
			lambda x: print_host(find_host(server, x)),
			["host_id"],
			_help="print all information about specified host"),
		Command(
			cmdline,
			"edit",
			lambda x: edit_host(find_host(server, x)),
			["host_id"],
			_help="edit fields in the host given its id"),
		# Command(cmdline, "new", lambda: ),
		Command(cmdline, "new", lambda: new_host(server), _help="create a new package environment"),
		Command(cmdline, "exit", exit, _help="exit"),
		Command(cmdline, "q", exit, _help="exit")
	]
	
	for cmd in commands:
		cmdline.new_command(cmd)
	cmdline.commandline()
	
	return 0


exit(main())
