import threading
from collections import namedtuple
import os, socket

ConnectionTup = namedtuple('ConnectionTup', ['sock_fd', 'client_addr'])
JobTup = namedtuple('JobTup', ['id', 'script', 'args', 'next', 'res'])


def start_job(job: JobTup):
	exec("from scripts import %s" % job.script)
	return eval("%s.main(%s)" % (job.script, job.args))


def get_string_bin(self, binary, start):
	for x in binary:
		pass

type_defines = {
	'i': lambda i, x: return struct.unpack("!i", x[i:i+4])
	's': lambda i, x: 
}

def struct_read (format_str, array):
	out = []
	current_index = 0
	for c in format_str:
		if c == 'i':
			out.append(struct.unpack())

class RequestQueue:
	def __init__(self):
		self.queue_mutex = threading.Lock()
		self.condition_mutex = threading.Lock()
		self.condition = threading.Condition(self.condition_mutex)
		
		self.head = None
		self.tail = None
	
	def handle_request(self, conn: ConnectionTup):
		data_array = []
		data = conn.sock_fd.recv(16)
		while data:
			data_array.append(data)
			data = conn.sock_fd.recv(16)
		

	def add_request(self, req: JobTup):
		self.queue_mutex.acquire()
		if self.head is None:
			self.head = req
			self.tail = req
		else:
			self.tail.next = req
			self.tail = req
		req.next = None  # Always the end
		self.queue_mutex.release()
		self.condition.notify()
	
	def get_next(self) -> JobTup:
		self.queue_mutex.acquire()
		return_val = self.head
		if self.head is None:
			return return_val
		self.head = self.head.next
		if self.head is None:  # That was the last item
			self.tail = None
		self.queue_mutex.release()


class Pool:
	def __init__(self, queue: RequestQueue, pool_index: int):
		self.pool_index = pool_index
		self.queue = queue
		self.pid = threading.get_ident()
		self.loop()
		self.kill = False
	
	def loop(self):
		while not self.kill:
			job = self.queue.get_next()
			if job is None:
				self.queue.condition.wait()
				continue
			job.res = start_job(job)


class WorkerServer:
	def __init__(self, pool_number: int = 8):
		self.pools = []
		self.request_stack = RequestQueue
		self.socket = None
		self.sock_addr = None
		
		for i in range(pool_number):
			self.pools.append(threading.Thread(target=Pool, args=(self, i)))
	
	def start(self, socket_name: str):
		self.sock_addr = socket_name
		try:
			os.unlink(self.sock_addr)
		except OSError:
			if os.path.exists(self.sock_addr):
				raise
		self.socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
		self.socket.bind(self.sock_addr)
	
	def server_loop(self):
		pass