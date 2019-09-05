from request import Address, Request


class Stream(Request):
	def __init__(self, adr: Address, args: list):
		super(Stream).__init__(adr, Request.REQ_JOB_STREAM, args, _ssl=True)
	
	def recv(self, chunksize=32):
		buffer = bytearray(chunksize)
		
		nbytes = self.client.socket.recv_into(buffer, chunksize)
		if nbytes <= 0:
			raise StopIteration()
		
		return buffer
