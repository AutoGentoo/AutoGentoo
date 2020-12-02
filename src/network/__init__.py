import struct
import sys
from typing import Union, List, Optional

from .autogentoo_network import *


def build_message(token: int, *args: Union[int, float], **kwargs) -> Message:
    if len(args) > 6:
        raise TypeError("Message only supports up to 6 parameters")

    def convert_to_bin(s) -> bytes:
        if isinstance(s, float):
            return struct.pack('d', s)
        elif isinstance(s, int):
            return s.to_bytes(8, signed=False, byteorder=sys.byteorder)

    parsed_args = []
    for arg in args:
        parsed_args.append(convert_to_bin(arg))

    for i in range(6 - len(args)):
        parsed_args.append(convert_to_bin(0))

    data_param: Optional[bytes] = None
    if 'data' in kwargs:
        data_param = kwargs['data']

    return Message((token,
                    parsed_args[0],
                    parsed_args[1],
                    parsed_args[2],
                    parsed_args[3],
                    parsed_args[4],
                    parsed_args[5],
                    data_param))
