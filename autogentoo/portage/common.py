from typing import TypeVar, List, Tuple, Generic

from autogentoo.cportage import Atom, UseFlag

T = TypeVar("T")


class DependencyContainer(Generic[T]):
    depend: List[T]
    run_depend: List[T]
    build_depend: List[T]
    post_depend: List[T]

    def __init__(self):
        self.depend = []
        self.run_depend = []
        self.build_depend = []
        self.post_depend = []

    def clear(self):
        self.depend = []
        self.run_depend = []
        self.build_depend = []
        self.post_depend = []

    def __getitem__(self, idx: int) -> List[T]:
        return (self.depend, self.run_depend, self.build_depend, self.post_depend)[idx]

    def __iter__(self) -> Tuple[List[T], List[T], List[T], List[T]]:
        return self.depend, self.run_depend, self.build_depend, self.post_depend


class RequiredUseException(Exception):
    pass


class ChangedUseException(Exception):
    useflag: UseFlag
    atom: Atom

    def __init__(self, atom: Atom, useflag: UseFlag):
        self.useflag = useflag
        self.atom = atom


class ResolutionException(Exception):
    pass
