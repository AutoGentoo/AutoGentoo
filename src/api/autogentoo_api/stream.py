from .request import Address, Request


class Stream(Request):
	def __init__(self, adr: Address, args: list):
		Request.__init__(self, adr, Request.REQ_JOB_STREAM, args, _ssl=True)
		self.send()

	def __iter__(self):
		return self

	def __next__(self, chunksize=32) -> str:
		buffer = bytearray(chunksize)
		
		nbytes = self.client.socket.recv_into(buffer, chunksize)
		if nbytes <= 0:
			raise StopIteration()
		
		return buffer.decode("utf-8")
