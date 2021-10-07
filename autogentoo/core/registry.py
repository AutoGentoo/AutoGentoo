from abc import ABC
from util.math import calculate_crc32


class CacheObject:
    data: bytes
    _pos = 0  # current data position

    def __init__(self, data: bytes):
        self.data = data
        self._pos = 0

    def _read_int(self) -> int:
        assert self._pos + 4 <= len(self.data), "Not enough bytes left in buffer"

        out = int.from_bytes(self.data[self._pos:self._pos + 4],
                             byteorder="little", signed=False)
        self._pos += 4
        return out

    def _read_string(self):
        pass

    def _read_header(self):
        assert self._pos == 0, "self._read_header() must be run first"
        assert len(self.data) > 4, "Invalid data length, expected over 4 bytes"

        # Validate the data using the header checksum
        checksum = calculate_crc32(self.data[4:])
        stored_checksum = self._read_int()
        assert checksum == stored_checksum, \
            "Failed to validate checksum %d != %d" % (checksum, stored_checksum)

        # Read the two length descriptors
        format_n = self._read_int()
        name_n = self._read_int()


class Cacheable(ABC):
    def __init__(self):
        pass

    def from_cache(self, cache_object: CacheObject):
        pass
