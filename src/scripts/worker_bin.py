from worker import Worker
from interface import Server
from op_socket import Address

import signal
import os


def main(args):
	parent = Server(Address("localhost", int(args[1])), False)
	worker = Worker(parent)
	
	print(os.getpid())
	
	signal.signal(signal.SIGUSR1, worker.flush_queue)
	worker.start()


if __name__ == "__main__":
	import sys
	main(sys.argv)
