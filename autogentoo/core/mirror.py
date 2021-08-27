"""
Manages dist-file mirror locations
"""
import enum
import urllib.request
from pathlib import Path


class Mirror:
    class Protocol(enum.Enum):
        HTTP = enum.auto()
        HTTPS = enum.auto()
        FTP = enum.auto()
        RSYNC = enum.auto()  # TODO(tumbar) Not supported yet

    def __init__(self, url: str):
        protocol_name = url.split(":", 1)[0].upper()
        self.protocol = Mirror.Protocol[protocol_name]
        self.url = url

    protocol: Protocol
    url: str

    def download(self, url_path: str, output_path: Path):
        file_path = self.url + url_path
        # urllib.request supports FTP, HTTP, HTTPS
        if self.protocol in (
            Mirror.Protocol.HTTP,
            Mirror.Protocol.HTTPS,
            Mirror.Protocol.FTP,
        ):
            urllib.request.urlretrieve(file_path, str(output_path))
        else:
            raise NotImplementedError("%s is not implemented yet" % self.protocol)


# Sue me
DEFAULT_MIRROR = Mirror("https://mirrors.rit.edu/gentoo/")
