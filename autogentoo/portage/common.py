import enum
from abc import ABC, abstractmethod
from dataclasses import dataclass
from typing import TypeVar, List, Generic, Iterable

from autogentoo.cportage import Atom, UseFlag

T = TypeVar("T")


class FlagType(enum.IntEnum):
    NORMAL = enum.auto()
    TEMP = enum.auto()
    # PRI_LOW = enum.auto()
    # PRI_HIGH = enum.auto()


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

    def __iter__(self) -> Iterable[List[T]]:
        return (
            self.depend,
            self.run_depend,
            self.build_depend,
            self.post_depend,
        ).__iter__()


class Suggestion(ABC):
    @abstractmethod
    def __hash__(self): ...


@dataclass(frozen=True)
class UseSuggestion(Suggestion):
    use_name: str
    value: bool

    def __repr__(self):
        if self.value:
            return self.use_name
        else:
            return "!%s" % self.use_name

    def __hash__(self):
        return hash(self.use_name) + hash(self.value)


class SuggestionExpression(Suggestion):
    class Operator(enum.IntEnum):
        LEAST_ONE = 0  # or
        EXACT_ONE = 1  # xor
        MOST_ONE = 2  # not x or (xor)
        AND = 3  # all

    operator: Operator
    suggestions: List[Suggestion]

    def __init__(self, operator: Operator):
        self.operator = operator
        self.suggestions = []

    def append(self, suggestion: Suggestion):
        self.suggestions.append(suggestion)

    def __iter__(self) -> Iterable[Suggestion]:
        return self.suggestions.__iter__()

    def __repr__(self) -> str:
        token_names = ["least-one", "one-of", "most-one", "all"]
        return "%s ( %s )" % (
            token_names[self.operator],
            " ".join(repr(x) for x in self.suggestions),
        )

    def __hash__(self):
        return self.operator + sum([hash(x) for x in self.suggestions])


class ChangedUseException(Exception):
    useflag: UseFlag
    atom: Atom

    def __init__(self, atom: Atom, useflag: UseFlag):
        self.useflag = useflag
        self.atom = atom


class ResolutionException(Exception):
    pass


class InvalidExpressionException(Exception):
    pass
