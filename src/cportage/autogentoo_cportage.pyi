from typing import Optional


class ID(int):
    pass


class AtomVersion:
    raw: str

    def __init__(self, version_string: str): ...
    def __cmp__(self, other: AtomVersion): ...


class UseFlag:
    name: str
    state: bool

    def __init__(self, name: str, state: bool): ...

class AtomFlag:
    name: str
    option: int
    default: int
    next: Optional['AtomFlag']

    def __init__(self, expr: str): ...
    def __next__(self) -> AtomFlag: ...
    def __iter__(self) -> AtomFlag: ...


class Portage:
    pass


class RequiredUse:
    id: ID
    operator: int
    depend: 'RequiredUse'
    next: 'RequiredUse'
    def __init__(self, required_use_string: str): ...


class Atom:
    id: ID
    category: str
    name: str

    key: str
    repository: str

    slot: str
    sub_slot: str
    slot_opts: int

    range: int
    blocks: int
    revision: int

    version: AtomVersion
    useflags: AtomFlag

    def __init__(self, atom_string: str): ...


class Dependency:
    use_operator: int
    use_condition: ID
    atom: Atom

    def __init__(self, depend_string: str): ...

    children: Optional['Dependency']
    next: Optional['Dependency']

    def __next__(self) -> 'Dependency': ...
    def __iter__(self) -> 'Dependency': ...

def init(portage: Portage): ...
