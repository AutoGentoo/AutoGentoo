from typing import Union, Optional
from pathlib import Path


_autogentoo_directory: Optional[Path] = None


def _init_fs():
    global _autogentoo_directory
    if _autogentoo_directory is None:
        _autogentoo_directory = Path(__file__).parent.parent


def get(path: Union[str, Path]) -> Path:
    if _autogentoo_directory is None:
        _init_fs()

    return _autogentoo_directory / path
