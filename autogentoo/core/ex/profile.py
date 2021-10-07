from dataclasses import dataclass
from typing import Set, List, Dict


@dataclass(frozen=True)
class Profile:
    """
    Describes a build configuration to
    be reusable on another container.
    """

    name: str
    author: str
    revision: int
    description: str

    architecture: str
    instruction_sets: List[str]

    world: List[str]
    files: Dict[str, str]
