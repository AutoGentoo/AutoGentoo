from abc import ABC, abstractmethod
from typing import List, Optional, Dict, Generator

from autogentoo.cportage import Dependency, Atom, Ebuild, UseFlag, UseOperatorT
from autogentoo.cportage.autogentoo_cportage import get_portage
from autogentoo.portage import (
    RequiredUseException,
    DependencyContainer,
    ResolutionException,
)
from autogentoo.portage.emerge import emerge_session


def resolve_single(
    parent: Optional["SelectedEbuild"], depend_expr: Dependency
) -> "ResolveDependency":
    if parent is None and depend_expr.atom is not None:
        raise ResolutionException(
            "Use condition expressions are not valid a global scope"
        )

    if depend_expr.atom is not None:  # Simple atom selection
        return emerge_session().select_atom(depend_expr.atom)
    else:
        assert depend_expr.use_condition == 0 and depend_expr.use_operator in (
            UseOperatorT.ENABLE,
            UseOperatorT.DISABLE,
        )

        if depend_expr.use_condition != 0:
            # Simple use condition
            use_flag = get_portage().get_use_flag(depend_expr.use_condition)
            use_flag = UseFlag(
                use_flag.name,
                True if depend_expr.use_condition == UseOperatorT.ENABLE else False,
            )

            assert depend_expr.children is not None, "Invalid dependency expression"
            parent.add_use_hook(use_flag, depend_expr.children)
        else:
            raise NotImplementedError("Complex use selection is not implemented yet")


def resolve_all(
    parent: Optional["SelectedEbuild"], depend: Dependency
) -> Generator["ResolveDependency", None, None]:
    for dep in depend:
        yield resolve_single(parent, dep)


class ResolveDependency(ABC):
    _is_dirty: bool

    def is_dirty(self) -> bool:
        return self._is_dirty

    @abstractmethod
    def get_resolved(self) -> Optional["ResolveDependency"]:
        ...


class Conditional(ResolveDependency, ABC):
    @abstractmethod
    def run_hook(self, arg):
        ...


class UseConditional(Conditional):
    parent: "SelectedEbuild"

    # Should we raise an error if this condition is ever false?
    required: bool

    # The target state required for this expression to be analyzed
    useflag: UseFlag

    # The guarded dependency used to re-calculated
    dependency: Optional[Dependency]

    current_evaluation: Optional["ResolveDependency"]

    def __init__(
        self,
        parent: "SelectedEbuild",
        useflag: UseFlag,
        expression: Optional[Dependency],
        required=False,
    ):
        self.parent = parent
        self.useflag = useflag
        self.required = required
        self.dependency = expression
        self.current_evaluation = None
        self._is_dirty = False

    def get_resolved(self) -> Optional["ResolveDependency"]:
        if self.dependency is None:
            return None

        if self._is_dirty or self.current_evaluation is None:
            self.current_evaluation = resolve_single(self.parent, self.dependency)

        return self.current_evaluation

    def run_hook(self, flag_state: bool):
        # Only evaluate the expression if our condition is met
        if self.useflag.state != flag_state:
            if self.required:
                raise RequiredUseException()

            # Mark this expression to re-evaluate the dependencies
            self._is_dirty = True
            self.current_evaluation = None
            return

        self._is_dirty = False


class SelectedEbuild(ResolveDependency):
    selected_by: List[Atom]

    # The original ebuild
    ebuild: Ebuild

    # The useflag delta from IUSE of the original ebuild
    useflags: Dict[str, UseFlag]

    # Expressions that depend on use flags from this ebuild
    use_conditions: Dict[str, List[UseConditional]]

    generators: DependencyContainer[ResolveDependency]
    resolved_deps: DependencyContainer[ResolveDependency]

    def __init__(self, atom: Atom, ebuild: Ebuild):
        self.selected_by = []
        self.ebuild = ebuild
        self.useflags = {}
        self.use_conditions = {}

        self.generators = DependencyContainer[ResolveDependency]()
        self.add_selected_by(atom)

    def get_resolved(self) -> Optional[ResolveDependency]:
        self.regenerate()
        return self

    def add_selected_by(self, atom: Atom):
        self.selected_by.append(atom)

    def change_within_slot(self, atom: Atom) -> bool:
        """
        Change the currently selected ebuild
        to another ebuild within the same slot
        :param atom: try to match an ebuild to every dependency+this
        :return: True it can be done
        """

        ebuild_match: Optional[Ebuild] = None
        for ebuild in self.ebuild.package:
            all_match = atom.matches(ebuild)
            for prev_atom in self.selected_by:
                all_match = prev_atom.matches(ebuild)
                if not all_match:
                    break

            if all_match:
                ebuild_match = ebuild
                break

        if ebuild_match is None:
            return False

        self._is_dirty = True
        self.ebuild = ebuild_match
        return True

    def regenerate(self):
        """
        Refresh all children
        :return:
        """

        if self._is_dirty:
            # A new ebuild was selected
            # We need to regenerate the generators
            self.generators.clear()

            for i, dep_type in enumerate(
                (
                    self.ebuild.bdepend,
                    self.ebuild.depend,
                    self.ebuild.rdepend,
                    self.ebuild.pdepend,
                )
            ):
                for dep in resolve_all(self, dep_type):
                    self.generators[i].append(dep)

            self._is_dirty = False

        # All non-dirty expressions are already cached
        # We can just remove everyone and re-append
        self.resolved_deps.clear()

        # Regenerate the dependency with the dirty flag enabled
        i = 0
        for dep_type in self.generators:
            for generator in dep_type:
                self.resolved_deps[i].append(generator.get_resolved())
            i += 1

    def get_use(self, name: str) -> UseFlag:
        if name not in self.useflags:
            return self.ebuild.iuse[name]
        return self.useflags[name]

    def set_required_use(self, useflag: UseFlag):
        self.add_use_hook(useflag, None, required=True)

    def add_use_hook(
        self, useflag: UseFlag, expression: Optional[Dependency], required: bool = False
    ):
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
