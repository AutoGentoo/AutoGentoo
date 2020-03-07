from autogentoo_api import DynamicBinary
import socket

class Request(DynamicBinary):
	
	
	def __init__(self, conn: socket.socket):
		super().__init__(False)
		
		self.data = conn.recv(1024)
		self.read_template("i")