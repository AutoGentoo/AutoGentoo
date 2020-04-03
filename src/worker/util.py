import socket
import struct
import sys
from typing import Union


class Streamable:
    def __init__(self, stream):
        self.stream = stream
    
    def read(self, n: int) -> bytes:
        return self.stream(n)

    def read_int(self) -> int:
        buf = self.read(4)
        if len(buf) != 4:
            return -1
    
        return struct.unpack('i', buf)[0]

    def read_str(self) -> Union[str, None]:
        length = self.read_int()
    
        if length == 0:
            return None
    
        string = self.read(int(length))
    
        return string.decode("utf-8")
    
    def write(self, b: bytes) -> int:
        return self.stream.write(b)
    
    def write_int(self, i: int) -> int:
        return self.write(i.to_bytes(4, sys.byteorder))

    def write_str(self, string: str) -> int:
        out = self.write_int(len(string))
        return out + self.write(string.encode('utf-8'))

    def close(self):
        self.stream.close()


class SockStream(Streamable):
    def __init__(self, sock: socket.socket):
        super(SockStream, self).__init__(sock)
    
    def read(self, n: int) -> bytes:
        return self.stream.recv(n)
    
    def write(self, b: bytes) -> int:
        return self.stream.send(b)
