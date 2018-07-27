from worker import Worker
from interface import Server
from op_socket import Address
import os, signal


def handle_usr1(i, stack):
	main_worker.flush_queue()


def main(args):
	signal.signal(signal.SIGUSR1, handle_usr1)
	signal.signal(signal.SIGINT, handle_usr1)
	parent = Server(Address("localhost", int(args[1])), False)
	
	global main_worker
	main_worker = Worker(parent)
	print(os.getpid())
	
	main_worker.start()


if __name__ == "__main__":
	import sys
	main(sys.argv)
