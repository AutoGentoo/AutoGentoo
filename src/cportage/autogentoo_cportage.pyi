class ID(int):
    pass


class AtomVersion:
    pass


class UseFlag:
    name: str
    state: bool

    def __init__(self, name: str, state: bool): ...


class Portage:
    pass


class RequiredUse:
    @staticmethod
    def parse(portage: Portage, use_string: str): ...


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
    useflags: UseFlag

    def __init__(self, atom_string: str): ...


class Dependency:
    use_operator: int
    use_condition: ID
    atom: Atom

    children: 'Dependency'
    next: 'Dependency'

    def __next__(self) -> 'Dependency': ...
    def __iter__(self) -> 'Dependency': ...

    @staticmethod
    def parse(portage: Portage, to_parse: str): ...
