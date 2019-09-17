import ctypes


class Semaphore:
	def __init__(self, file="/tmp/autogentoo.config.semaphore"):
		self.file = file
		
		self.pthread_lib = ctypes.CDLL("pthread.so")
		
		self.__sem_wait = self.pthread_lib.sem_wait
		self.__sem_wait.argtypes = [ctypes.c_void_p]
		self.__sem_wait.restype = ctypes.c_int
		
		self.__sem_post = self.pthread_lib.sem_post
		self.__sem_post.argtypes = [ctypes.c_void_p]
		self.__sem_post.restype = ctypes.c_int
		
		self.__sem_open = self.pthread_lib.sem_open
		self.__sem_open.argtypes = [ctypes.c_char_p, ctypes.c_int]
		self.__sem_open.restype = ctypes.c_void_p
		
		self.__sem_close = self.pthread_lib.sem_close
		self.__sem_close.argtypes = [ctypes.c_void_p]
		self.__sem_close.restype = [ctypes.c_int]
		
		self.__sem__ = self.__sem_open(self.file.encode("utf-8"), 0)
	
	def wait(self):
		self.__sem_wait(self.__sem__)
	
	def post(self):
		self.__sem_post(self.__sem__)
	
	def __del__(self):
		self.__sem_close(self.__sem__)
