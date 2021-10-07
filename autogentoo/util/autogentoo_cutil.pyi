from typing import Union


def calculate_crc32(data: Union[bytes, bytearray, memoryview]) -> int:
    """
    Calculate the CRC32 checksum for a block of data
    :param data: block of data to calculate checksum for
    :return: Unsigned 32-bit checksum
    """
