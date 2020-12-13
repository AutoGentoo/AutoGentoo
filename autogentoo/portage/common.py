import enum
from dataclasses import dataclass
from typing import TypeVar, List, Tuple, Generic, Iterable, Union, Optional

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


class Suggestion:
    pass


@dataclass(frozen=True)
class UseSuggestion(Suggestion):
    use_name: str
    value: bool


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


class RequiredUseException(Exception):
    suggestion: Union[None, Suggestion]

    def __init__(self, suggestion: Optional[Suggestion] = None):
        self.suggestion = suggestion

    def has_suggestion(self) -> bool:
        return self.suggestion is not None


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
