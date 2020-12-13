from abc import ABC, abstractmethod
from typing import List, Optional, Dict, Generator

from autogentoo.cportage import get_portage, Dependency, Atom, Ebuild, UseFlag, UseOperatorT, Portage, \
    AtomUseT, AtomUseDefaultT
from autogentoo.portage import (
    RequiredUseException,
    DependencyContainer,
    ResolutionException,
)

__emerge_session__: Optional['Emerge'] = None


def emerge_init(emerge: 'Emerge'):
    global __emerge_session__
    __emerge_session__ = emerge


def emerge_session() -> 'Emerge':
    global __emerge_session__
    if __emerge_session__ is None:
        raise RuntimeError(
            "emerge_init() must be called before emerge_session() is called"
        )

    return __emerge_session__


def resolve_single(
    parent: Optional["SelectedEbuild"], depend_expr: Dependency
) -> "ResolveDependency":
    if parent is None and depend_expr.atom is None:
        raise ResolutionException(
            "Use condition expressions are not valid at global scope"
        )

    if depend_expr.atom is not None:  # Simple atom selection
        sel_ebuild = emerge_session().select_atom(depend_expr.atom)

        if depend_expr.atom.useflags is None:
            return sel_ebuild

        for use in depend_expr.atom.useflags:
            if not sel_ebuild.has_use(use.name):
                # Use the use default because it doesn't exist in
                # in the IUSE

                default = AtomUseDefaultT(use.default)
                if default == AtomUseDefaultT.NONE:
                    # We have no fallback when this useflag doesn't exist
                    # This is an error
                    raise KeyError("Invalid use flag '%s' for atom '%s'" % (use.name, depend_expr.atom))

                atom_flag = AtomUseT.ENABLE if default == AtomUseDefaultT.ON else AtomUseT.DISABLE
                sel_ebuild.add_use_requirement(use.name, atom_flag)
            else:
                sel_ebuild.add_use_requirement(use.name, AtomUseT(use.option))

        return sel_ebuild
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

            conditional = UseConditional(parent, use_flag, depend_expr.children)
            parent.add_use_hook(use_flag, conditional)
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


class Hookable(ResolveDependency, ABC):
    @abstractmethod
    def run_hook(self, arg):
        ...


class UseSelection(Hookable):

    enforcing: bool
    target_value: bool
    parent: 'SelectedEbuild'
    use_flag: UseFlag
    flag: AtomUseT

    def __init__(self, parent: 'SelectedEbuild', use_flag: UseFlag, flag: AtomUseT):
        self.parent = parent
        self.use_flag = use_flag
        self.flag = flag
        self.target_value = parent.get_use(use_flag.name).state
        self.enforcing = False

        if self.flag == AtomUseT.ENABLE:
            self.target_value = True
            self.enforcing = True
        elif self.flag == AtomUseT.DISABLE:
            self.target_value = False
            self.enforcing = True
        elif self.flag == AtomUseT.DISABLE_IF_OFF:
            if not self.target_value:
                self.enforcing = True
        elif self.flag == AtomUseT.ENABLE_IF_ON:
            if self.target_value:
                self.enforcing = True
        elif self.flag == AtomUseT.EQUAL:
            self.enforcing = True
        elif self.flag == AtomUseT.OPPOSITE:
            self.enforcing = True
            self.target_value = not self.target_value
            parent.set_use(UseFlag(use_flag.name, self.target_value))

    def run_hook(self, arg: bool):
        """
        Make sure that this flag will
        not get an invalid value
        :param arg:
        :return:
        """

        if self.enforcing:
            if arg != self.target_value:
                raise RequiredUseException()

    def get_resolved(self) -> Optional["ResolveDependency"]:
        # This hook does not run any
        # dependency resolution
        return None


class UseConditional(Hookable):
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

    # Use requirements
    use_requirements: Dict[str, AtomUseT]

    # Triggers that run when a use flag is changes
    use_flag_hooks: Dict[str, List[Hookable]]

    generators: DependencyContainer[ResolveDependency]
    resolved_deps: DependencyContainer[ResolveDependency]

    def __init__(self, atom: Atom, ebuild: Ebuild):
        self.selected_by = []
        self.ebuild = ebuild
        self.useflags = {}
        self.use_flag_hooks = {}

        self.generators = DependencyContainer[ResolveDependency]()

        self._is_dirty = True
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
            if not self.ebuild.metadata_init:
                self.ebuild.initialize_metadata()

            for i, dep_type in enumerate(
                (
                    self.ebuild.bdepend,
                    self.ebuild.depend,
                    self.ebuild.rdepend,
                    self.ebuild.pdepend,
                )
            ):
                if dep_type is None:
                    continue
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
                resolved = generator.get_resolved()
                if resolved is None:
                    continue
                self.resolved_deps[i].append(resolved)
            i += 1

    def has_use(self, name: str) -> bool:
        return name in self.useflags or name in self.ebuild.iuse

    def get_use(self, name: str) -> UseFlag:
        if name not in self.useflags:
            return self.ebuild.iuse[name]
        return self.useflags[name]

    def add_use_requirement(self, name: str, flag: AtomUseT):
        """
        Select a use flag required by an atom
        :param name:
        :param flag:
        :return:
        """
        pass

    def add_use_hook(
        self, useflag: UseFlag, hook: Hookable
    ):
        """
        Associate an action with a flag changing
        :param useflag:
        :param hook:
        :return:
        """
        if useflag.name not in self.use_flag_hooks:
            self.use_flag_hooks[useflag.name] = []

        self.use_flag_hooks[useflag.name].append(hook)
        hook.run_hook(self.get_use(useflag.name).state)

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
            if useflag.name in self.use_flag_hooks:
                for hook in self.use_flag_hooks[useflag.name]:
                    hook.run_hook(useflag.state)

            self.useflags[useflag.name] = useflag


class InstallPackage:
    key: str
    selected_ebuild_slots: Dict[str, SelectedEbuild]

    def __init__(self, key: str):
        self.key = key
        self.selected_ebuild_slots = {}

    def match_atom(self, atom: Atom, ebuild: Ebuild) -> SelectedEbuild:
        # Check if slot has already been selected
        if ebuild.slot in self.selected_ebuild_slots:
            # See if this atom matches the selected ebuild
            sel_ebuild = self.selected_ebuild_slots[ebuild.slot]
            if atom.matches(sel_ebuild.ebuild):
                return sel_ebuild  # We're good

            # See if we can change the selected ebuild to match this atom
            if sel_ebuild.change_within_slot(atom):
                return sel_ebuild  # Works!
            else:
                raise NotImplementedError("Cannot split a slot into multi-slot yet!")
        else:
            # See if this atom matches any of the currently scheduled slots
            for key, sel_ebuild in self.selected_ebuild_slots.items():
                if atom.matches(sel_ebuild.ebuild):
                    return sel_ebuild

            # We need to create a new selected ebuild and add it here
            return self.add_atom(atom, ebuild)

    def add_atom(self, atom: Atom, ebuild: Ebuild) -> SelectedEbuild:
        """
        Add a SelectedEbuild in its slot
        :param atom: atom that selected with ebuild
        :param ebuild: ebuild selected by atom
        :return: SelectedEbuild generated from the atom+ebuild
        """

        sel_ebuild = SelectedEbuild(atom, ebuild)
        self.selected_ebuild_slots[sel_ebuild.ebuild.slot] = sel_ebuild
        return sel_ebuild


class Emerge:
    portage: Portage
    selected_packages: Dict[str, InstallPackage]

    def __init__(self):
        self.portage = get_portage()
        self.selected_packages = {}

    def select_atom(self, atom: Atom) -> SelectedEbuild:
        ebuild = self.portage.match_atom(atom)
        if ebuild is None:
            raise ResolutionException("No ebuild to match '%s' could be found" % atom)

        if ebuild.key in self.selected_packages:
            # Ebuild with this key has already been selected
            # See if we can match this to an existing slot
            install_pkg = self.selected_packages[ebuild.key]
            return install_pkg.match_atom(atom, ebuild)
        else:
            pkg = InstallPackage(atom.key)
            self.selected_packages[pkg.key] = pkg
            return pkg.add_atom(atom, ebuild)
