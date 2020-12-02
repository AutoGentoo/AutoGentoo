from typing import Union, Callable, Tuple, Optional


class Message:
    token: int
    val1: bytes
    val2: bytes
    val3: bytes
    val4: bytes
    val5: bytes
    val6: bytes
    data: Optional[bytes]

    def __init__(self, args: Tuple[int, bytes, bytes, bytes, bytes, bytes, bytes, Optional[bytes]]): ...

class TCPServer:
    def __init__(self, address: Union[str, int]): ...
    def start(self): ...
    def stop(self): ...
    def set_request_callback(self, callback: Callable[[Message], Message]): ...


def send_message(address: Union[Tuple[int, int], str], message: Message) -> Message:...
