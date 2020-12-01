from typing import NamedTuple, Union, Callable, Optional
from .autogentoo_network import *


class Message(NamedTuple):
    """
    These are not actually stubs but actually
    the tuples that are the used in request parsing
    """
    token: int
    val1: bytes
    val2: bytes
    val3: bytes
    val4: bytes
    val5: bytes
    val6: bytes


class MessageFrame(Message):
    data: bytes


class AutoGentooTCPServer(TCPServer):
    """
    Wrap the C TCP Server to handle
    callbacks with the above named tuples
    """

    callback: Optional[Callable[[Union[Message, MessageFrame]], None]]

    def __init__(self, address: Union[str, int]):
        super().__init__(address)
        super().set_request_callback(self.request_handler)

        self.callback = None

    def set_request_callback(self, callback: Callable[[Union[Message, MessageFrame]], None]):
        self.callback = callback

    def request_handler(self, request: tuple):
        # Check if we have a callback set up
        if self.callback is None:
            raise AttributeError("A callback needs to be set up!")

        # Convert the request to one of the named tuples
        if len(request) == 7:
            self.callback(Message(*request))
        elif len(request) == 8:
            self.callback(MessageFrame(*request))
        else:
            raise TypeError("Invalid request tuple of length: %d" % len(request))
