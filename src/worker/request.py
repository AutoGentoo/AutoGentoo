import importlib
import sys

from autogentoo_api.dynamic_binary import BufferedDynamicBinary
import socket
from client import Host
from log import Log
from script import *
from threading import Thread

logfp = None


class Request(BufferedDynamicBinary, Thread):
	script_dir = "/autogentoo/worker/"
	host: Host
	
	ERROR_HOST_READ = 1
	ERROR_JOB_INIT = 2
	
	def __init__(self, conn: socket.socket, log: Log, end_callback):
		super().__init__(conn)
		
		self.log = log
		
		self.conn = conn
		self.host = Host(conn)
		
		self.error = False
		self.job_name = ""
		self.script = ""
		self.pid = -1
		self.jobnum = -1
		self.args = []
		self.end_callback = end_callback
	
	@staticmethod
	def get_jobnum():
		job_max = 0
		for f in os.listdir("%s/logs/" % Request.script_dir):
			if os.path.isfile(os.path.join(Request.script_dir, f)) and f.startswith("job-"):
				c_job = int(f[4:f.rfind(".log")])
				if c_job > job_max:
					job_max = c_job
		
		return job_max + 1
	
	def handle(self):
		"""
		Negatives for job number indicate the error code
		Positives are the job number
		
		:return: the positive error code will be returned (0 for none)
		"""
		
		self.host.read_host()
		
		res = 0
		if self.host.id != "":
			self.log.error("Failed to read host from request")
			res = 1
		
		self.conn.send(res.to_bytes(4, 'big'))
		
		if res != 0:
			self.error = True
			return self.ERROR_HOST_READ
		
		# Read the request and generate a job number
		error_or_job_num = self.read_request()
		
		# Send the generated job number or error code to client
		self.conn.send(error_or_job_num.to_bytes(4, "big"))
		if error_or_job_num < 0:
			self.error = True
			return -1 * error_or_job_num
		
		return 0
	
	def read_request(self):
		self.buffer_read()
		
		self.script, self.args = self.read_template("sa(s)")
		if not os.path.isfile(os.path.join(self.script_dir, self.script)):
			self.log.error("No script named '%s' in ", self.script)
			return -1 * self.ERROR_JOB_INIT
		
		self.jobnum = self.get_jobnum()
	
	"""
	Extends Thread therefore this will run
	in its own thread upon start() being called
	
	Fork the current session and have parent wait
	for child worker to finish
	
	Lock file will signal any listening threads
	that the job has finished
	"""
	def run(self):
		logfile = "logs/%s.log" % self.job_name
		touch(logfile + ".lck")
		
		script_pid = os.fork()
		if script_pid == 0:
			try:
				module = importlib.import_module("scripts.%s" % self.script)
			except ModuleNotFoundError:
				self.log.error("Failed to load script '%s'", self.script)
				return -404
			
			# Run the script to load the functions
			importlib.reload(module)
			
			global logfp
			logfp = open("%s/logs/job-%d.log" % (self.script_dir, self.jobnum), "a+")
			sys.stdout = logfp
			sys.stderr = logfp
			
			try:
				ret = module.script(self.job_name, self.host, self.args)
			except Exception:
				traceback.print_exc()
				ret = 1
			
			logfp.close()
			
			exit(ret)
		else:
			pid, status = os.waitpid(script_pid, 0)
			rm(logfile + ".lck")
			
			self.end_callback(self)
