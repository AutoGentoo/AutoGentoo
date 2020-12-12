from abc import ABC, abstractmethod
from typing import List, Optional, Dict, TypeVar, Generic, Tuple

from autogentoo.cportage import Dependency, Atom, Ebuild, UseFlag, UseOperatorT
from autogentoo.cportage.autogentoo_cportage import get_portage

T = TypeVar('T')


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


def resolve_atom(atom: Atom) -> 'SelectedEbuild':
    ebuild = get_portage().match_atom(atom)
    if ebuild is None:
        raise ResolutionException("No ebuild to match '%s' could be found" % atom)

    selected_ebuild = SelectedEbuild(ebuild)
    return selected_ebuild


def resolve_dependency(parent: Optional['SelectedEbuild'], depend: Dependency) -> List['SelectedEbuild']:
    def resolve_single(depend_expr: Dependency) -> 'SelectedEbuild':
        if parent is None and depend_expr.atom is not None:
            raise ResolutionException("Use condition expressions are not valid a global scope")

        if depend_expr.atom is not None:  # Simple atom selection
            return resolve_atom(depend_expr.atom)
        else:
            if depend_expr.use_condition != 0:
                # Simple use condition
                use_flag = get_portage().get_use_flag(depend_expr.use_condition)
                use_flag = UseFlag(use_flag.name, True if depend_expr.use_condition == UseOperatorT.ENABLE else False)

                assert depend_expr.children is not None, "Invalid dependency expression"
                parent.add_use_hook(use_flag, depend_expr.children)
            else:
                raise NotImplementedError("Complex use selection is not implemented yet")

    deps = []
    for dep in depend:
        deps.append(resolve_single(dep))
    return deps


class Conditional(ABC):
    @abstractmethod
    def get_resolved(self) -> Optional[List['SelectedEbuild']]: ...

    @abstractmethod
    def run_hook(self, arg): ...


class UseConditional(Conditional):
    is_dirty: bool

    parent: 'SelectedEbuild'

    # Should we raise an error if this condition is ever false?
    required: bool

    # The target state required for this expression to be analyzed
    useflag: UseFlag

    # The guarded dependency used to re-calculated
    dependency: Optional[Dependency]

    current_evaluation: Optional[List['SelectedEbuild']]

    def __init__(self, parent: 'SelectedEbuild', useflag: UseFlag, expression: Optional[Dependency], required=False):
        self.parent = parent
        self.useflag = useflag
        self.required = required
        self.dependency = expression
        self.current_evaluation = None
        self.is_dirty = False

    def get_resolved(self) -> Optional[List['SelectedEbuild']]:
        if self.dependency is None:
            return None

        if self.is_dirty or self.current_evaluation is None:
            self.current_evaluation = resolve_dependency(self.parent, self.dependency)

        return self.current_evaluation

    def run_hook(self, flag_state: bool):
        # Only evaluate the expression if our condition is met
        if self.useflag.state != flag_state:
            if self.required:
                raise RequiredUseException()

            # Mark this expression to re-evaluate the dependencies
            self.is_dirty = True
            self.current_evaluation = None
            return

        self.is_dirty = False


class InstallSlot:
    def __init__(self):
        self.selected_ebuild_slots = set()

    def select_ebuild(self, ebuild: Ebuild):
        pass

    def select_ebuild_with_use_condition(self,
                                         ebuild: Ebuild,
                                         dependency_select: Dependency):
        pass


class SelectedEbuild:
    # The original ebuild
    ebuild: Ebuild

    # The useflag delta from IUSE of the original ebuild
    useflags: Dict[str, UseFlag]

    # Expressions that depend on use flags from this ebuild
    use_conditions: Dict[str, List[UseConditional]]

    non_conditional: DependencyContainer['SelectedEbuild']
    conditional: DependencyContainer[Conditional]

    def __init__(self, ebuild: Ebuild):
        self.ebuild = ebuild
        self.useflags = {}
        self.use_conditions = {}
        self.non_conditional = DependencyContainer['SelectedEbuild']()
        self.conditional = DependencyContainer[Conditional]()

    def regenerate(self):
        """
        Called from general IPC loop.
        This will cause
        :return:
        """

        # Remove expressions that need to be regenerated
        for dep_type in self.conditional:
            to_regen = []

            n = len(dep_type)
            i = 0
            while i < n:
                if dep_type[i].get_resolved() is None:
                    n -= 1
                    to_regen.append(dep_type[i])
                    del dep_type[i]
                else:
                    i += 1

    def get_use(self, name: str) -> UseFlag:
        if name not in self.useflags:
            return self.ebuild.iuse[name]
        return self.useflags[name]

    def set_required_use(self, useflag: UseFlag):
        self.add_use_hook(useflag, None, required=True)

    def add_use_hook(self, useflag: UseFlag, expression: Optional[Dependency], required: bool = False):
        """
        Associate an action with a flag changing
        :param useflag:
        :param expression:
        :param required:
        :return:
        """
        if useflag.name not in self.use_conditions:
            self.use_conditions[useflag.name] = []

        conditional = UseConditional(self, useflag, expression, required)
        self.use_conditions[useflag.name].append(conditional)
        conditional.run_hook(self.get_use(useflag.name).state)

    def set_use(self, useflag: UseFlag):
        """
        Enable a or disable a use flag for
        an ebuild being installed. Run all of the
        dependency hooks associated with this flag
        :param useflag: useflag to enable/disable
        :return: None
        """

        # Only run the hooks if there is a change in state
        if self.get_use(useflag.name).state != useflag.state:
            # Run all of the use-hooks for this flag
            if useflag.name in self.use_conditions:
                for hook in self.use_conditions[useflag.name]:
                    hook.run_hook(useflag.state)

            self.useflags[useflag.name] = useflag
