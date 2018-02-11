"""
The client interface to the server daemon

"""


class CommandManager:
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
		print("%s\nCommands:\n" % (self.name + ("\n" + self.help_text) if len(self.help_text) > 0 else ""))
		for key in self.commands:
			self.commands[key].print_help()

	def commandline(self):
		while self.keep_going:
			k = input("autogentoo > ").split(" ")
			try:
				self.commands[k[0]]
			except KeyError:
				print("command '%s' not found" % k[0])
				continue
			target_cmd = self.commands[k[0]]
			if len(k[0]) - 1 != target_cmd.argc:
				print("'%s' requires %s arguments" % (k[0], target_cmd.argc))
				continue

			target_cmd.run(k[1:])


class Command:
	selector = ""
	argc = 0
	argv = []
	help = ""
	manager = None
	fptr = None

	def __init__(self, manager, selector, func, argv=None, help=""):
		self.selector = selector
		self.argc = len(argv)
		self.argv = argv
		self.help = help
		self.manager = manager
		self.fptr = func

	def print_help(self):
		print("\t%s%s%s%s" % (self.selector, ', '.join(self.argv), ' ' * (self.manager.help_size - len(self.selector)), self.help))

	def run(self, args):
		self.fptr(*args)

class SocketRequest:
	pass

class ServerInterface:
	pass

def main (argv):
	cmd_man = CommandManager("autogentoo", 20, "AutoGentoo CLI")
	cmd_man.new_command(Command (cmd_man, "list", ))