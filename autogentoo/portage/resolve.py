import warnings
from abc import ABC, abstractmethod
from queue import Queue
from typing import List, Optional, Dict, Generator, Tuple, Set

from autogentoo.cportage import (
    get_portage,
    Dependency,
    Atom,
    Ebuild,
    UseFlag,
    UseOperatorT,
    Portage,
    AtomUseT,
    AtomUseDefaultT,
    RequiredUse,
    AtomBlockT,
)
from autogentoo.portage import (
    RequiredUseException,
    DependencyContainer,
    ResolutionException,
    UseSuggestion,
    SuggestionExpression,
    InvalidExpressionException,
)

__emerge_session__: Optional["Emerge"] = None


def emerge_init(emerge: "Emerge"):
    global __emerge_session__
    __emerge_session__ = emerge


def emerge_session() -> "Emerge":
    global __emerge_session__
    if __emerge_session__ is None:
        raise RuntimeError(
            "emerge_init() must be called before emerge_session() is called"
        )

    return __emerge_session__


def resolve_single(
    parent: Optional["SelectedEbuild"], depend_expr: Dependency
) -> Optional["ResolveDependency"]:
    if parent is None and depend_expr.atom is None:
        raise ResolutionException(
            "Use condition expressions are not valid at global scope"
        )

    if depend_expr.atom is not None:  # Simple atom selection
        # Check if this is a blocker
        if depend_expr.atom.blocks != AtomBlockT.NONE:
            emerge_session().add_block(parent, depend_expr.atom)
            return None

        sel_ebuild = emerge_session().select_atom(parent, depend_expr.atom)

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
                    raise KeyError(
                        "Invalid use flag '%s' for atom '%s'"
                        % (use.name, depend_expr.atom)
                    )

                atom_flag = (
                    AtomUseT.ENABLE
                    if default == AtomUseDefaultT.ON
                    else AtomUseT.DISABLE
                )

                sel_ebuild.add_use(use.name, atom_flag == AtomUseT.ENABLE)
                sel_ebuild.add_use_requirement(use.name, atom_flag)
            else:
                sel_ebuild.add_use_requirement(use.name, AtomUseT(use.option))

        return sel_ebuild
    else:
        if depend_expr.use_condition != 0:
            assert depend_expr.use_operator in (
                UseOperatorT.ENABLE,
                UseOperatorT.DISABLE,
            )

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
            warnings.warn(
                "Complex use selection is not implemented yet (%s)" % parent.ebuild.key
            )


def resolve_all(
    parent: Optional["SelectedEbuild"], depend: Dependency
) -> Generator["ResolveDependency", None, None]:
    for dep in depend:
        resolved = resolve_single(parent, dep)
        if resolved is not None:
            yield resolved


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
    parent: "SelectedEbuild"
    use_flag: UseFlag
    flag: AtomUseT

    def __init__(self, parent: "SelectedEbuild", use_flag: UseFlag, flag: AtomUseT):
        self.parent = parent
        self.use_flag = use_flag
        self.flag = flag
        self.target_value = use_flag.state
        self.enforcing = False

        if self.flag == AtomUseT.ENABLE:
            self.target_value = True
            self.enforcing = True
            parent.schedule_use(UseFlag(use_flag.name, True))
        elif self.flag == AtomUseT.DISABLE:
            self.target_value = False
            self.enforcing = True
            parent.schedule_use(UseFlag(use_flag.name, False))
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
            parent.schedule_use(UseFlag(use_flag.name, self.target_value))

    def run_hook(self, arg: bool):
        """
        Make sure that this flag will
        not get an invalid value
        :param arg:
        :return:
        """

        if self.enforcing:
            if arg != self.target_value:
                raise RequiredUseException(
                    UseSuggestion(self.use_flag.name, self.target_value)
                )

    def get_resolved(self) -> Optional["ResolveDependency"]:
        # This hook does not run any
        # dependency resolution
        return None


class RequiredUseHook(Hookable):
    expression: RequiredUse
    ebuild: "SelectedEbuild"

    def __init__(self, selected_ebuild: "SelectedEbuild", required_use: RequiredUse):
        self.expression = required_use
        self.ebuild = selected_ebuild

    def run_hook(self, arg: bool):
        """
        Evaluate the flags in the required use
        expression to make sure we have a match
        :param arg: flag state that changed (unused)
        :return: None
        """

        def evaluate_required_use(
            operator: SuggestionExpression.Operator, expr: RequiredUse
        ) -> Tuple[SuggestionExpression, int, int]:
            """
            Count the number of expressions
            that evaluate to True.
            :param operator: operator for suggestion
            :param expr: expression to verify
            :return: (suggestions, num_true, total)
            """

            n = 0
            k = 0
            suggestion = SuggestionExpression(operator)

            for req_use in expr:
                n += 1
                op = UseOperatorT(req_use.operator)
                if op == UseOperatorT.ENABLE or op == UseOperatorT.DISABLE:
                    target = op == UseOperatorT.ENABLE
                    state = self.ebuild.get_use(req_use.name).state

                    if req_use.depend is None and state == target:
                        k += 1
                    elif state == target:
                        # This is a conditional expression
                        child_suggestion, k_c, n_c = evaluate_required_use(
                            SuggestionExpression.Operator.AND, req_use.depend
                        )
                        if k_c == n_c:
                            k += 1
                        else:
                            # There are two different options here
                            # Either disable this useflag
                            # or try to meet its requirements
                            s = SuggestionExpression(
                                SuggestionExpression.Operator.LEAST_ONE
                            )
                            s.append(UseSuggestion(req_use.name, not state))
                            s.append(child_suggestion)
                            suggestion.append(s)
                    elif req_use.depend is not None and state == target:
                        k += 1
                    else:
                        suggestion.append(UseSuggestion(req_use.name, not state))
                elif op == UseOperatorT.LEAST_ONE:
                    child_suggestion, k_c, n_c = evaluate_required_use(
                        SuggestionExpression.Operator.LEAST_ONE, req_use.depend
                    )
                    if k_c >= 1:
                        k += 1
                    else:
                        suggestion.append(child_suggestion)
                elif op == UseOperatorT.EXACT_ONE:
                    child_suggestion, k_c, n_c = evaluate_required_use(
                        SuggestionExpression.Operator.EXACT_ONE, req_use.depend
                    )
                    if k_c == 1:
                        k += 1
                    else:
                        suggestion.append(child_suggestion)
                elif op == UseOperatorT.MOST_ONE:
                    child_suggestion, k_c, n_c = evaluate_required_use(
                        SuggestionExpression.Operator.MOST_ONE, req_use.depend
                    )
                    if k_c <= 1:
                        k += 1
                    else:
                        suggestion.append(child_suggestion)
                else:
                    raise InvalidExpressionException(
                        "Required use operator '%s' is not valid" % op
                    )

            return suggestion, k, n

        suggestions, g_k, g_n = evaluate_required_use(
            SuggestionExpression.Operator.AND, self.expression
        )
        if g_k != g_n:
            print(
                UseOperatorT(self.expression.operator),
                self.expression.name,
                self.ebuild,
            )
            print("%d %d" % (g_k, g_n), flush=True)
            raise RequiredUseException(suggestions)

    def get_resolved(self) -> Optional["ResolveDependency"]:
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
                raise RequiredUseException(
                    UseSuggestion(self.useflag.name, self.useflag.state)
                )

            # Mark this expression to re-evaluate the dependencies
            self._is_dirty = True
            self.current_evaluation = None
            return

        self._is_dirty = False


class SelectedEbuild(ResolveDependency):
    selected_by: Dict[Atom, Optional["SelectedEbuild"]]

    # The original ebuild
    ebuild: Ebuild

    # The useflag delta from IUSE of the original ebuild
    useflags: Dict[str, UseFlag]

    # Use requirements
    use_requirements: Dict[str, AtomUseT]

    # Triggers that run when a use flag is changes
    use_flag_hooks: Dict[str, List[Hookable]]
    global_flag_hooks: List[Hookable]  # Trigger when any use flag is changed

    # Flags to set next time we regenerate
    flags: Queue[UseFlag]

    generators: DependencyContainer[ResolveDependency]
    resolved_deps: DependencyContainer[ResolveDependency]
    resolve_session: "PackageResolutionSession"

    def __init__(
        self,
        parent: Optional["SelectedEbuild"],
        atom: Atom,
        ebuild: Ebuild,
        resolve_session: "PackageResolutionSession",
    ):
        self.selected_by = {}

        self.ebuild = ebuild
        self.useflags = {}
        self.use_flag_hooks = {}
        self.global_flag_hooks = []
        self.flags = Queue()
        self.resolve_session = resolve_session

        self.generators = DependencyContainer[ResolveDependency]()
        self.resolved_deps = DependencyContainer[ResolveDependency]()

        self._is_dirty = True
        self.add_selected_by(parent, atom)

    def get_resolved(self) -> Optional[ResolveDependency]:
        if self.resolve_session.check_resolved(self):
            return self

        self.resolve_session.add_to_session(self)
        self.regenerate()
        return self

    def add_selected_by(self, parent: Optional["SelectedEbuild"], atom: Atom):
        self.selected_by[atom] = parent

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

            # Update use flags by flushing the flag buffer
            self.flush_use()

            # Make sure all use-flags conform to requirements
            if self.ebuild.required_use is not None:
                self.add_use_hook(None, RequiredUseHook(self, self.ebuild.required_use))

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

        # Regenerate children (recursively)
        # All non-dirty expressions are already cached
        # We can just remove everyone and re-append
        self.resolved_deps.clear()

        # Regenerate the dependency with the dirty flag enabled
        i = 0
        for dep_type in self.generators:
            for generator in dep_type:
                if generator is None:
                    continue

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

    def add_use(self, name: str, value: bool):
        """
        Add a non-existent useflag
        :param name: name of useflag
        :param value: default value
        :return:
        """

        self.useflags[name] = UseFlag(name, value)

    def add_use_requirement(self, name: str, flag: AtomUseT):
        """
        Select a use flag required by an atom
        :param name: name of the atom flag
        :param flag: atom flag setting
        :return:
        """

        use = self.get_use(name)
        self.add_use_hook(use, UseSelection(self, use, flag))

    def add_use_hook(self, useflag: Optional[UseFlag], hook: Hookable):
        """
        Associate an action with a flag changing
        :param useflag: useflag to run hook on, None for all useflags
        :param hook: action to take when useflag is changed
        :return:
        """

        if useflag is None:
            self.global_flag_hooks.append(hook)
            hook.run_hook(None)
            return

        if useflag.name not in self.use_flag_hooks:
            self.use_flag_hooks[useflag.name] = []

        self.use_flag_hooks[useflag.name].append(hook)
        hook.run_hook(self.get_use(useflag.name).state)

    def schedule_use(self, useflag: UseFlag):
        """
        Update a useflag upon regeneration
        :param useflag: useflag to update
        :return: None
        """

        self.flags.put(useflag)
        self._is_dirty = True

    def flush_use(self):
        """
        Update all of the buffered useflags
        and run their change hooks.
        :return: None
        """

        while not self.flags.empty():
            useflag = self.flags.get()

            # Only run the hooks if there is a change in state
            if self.get_use(useflag.name).state != useflag.state:
                self.useflags[useflag.name] = useflag

                # Run all of the use-hooks for this flag
                if useflag.name in self.use_flag_hooks:
                    for hook in self.use_flag_hooks[useflag.name]:
                        hook.run_hook(useflag.state)

                # Run global use-hooks
                for hook in self.global_flag_hooks:
                    hook.run_hook(None)

    def __hash__(self):
        return id(self)

    def __repr__(self):
        return "SelectedEbuild<%s %s>" % (self.ebuild.key, id(self))


class InstallPackage:
    key: str
    selected_ebuild_slots: Dict[str, SelectedEbuild]
    resolve_session: "PackageResolutionSession"

    def __init__(self, key: str, resolve_session: "PackageResolutionSession"):
        self.key = key
        self.selected_ebuild_slots = {}
        self.resolve_session = resolve_session

    def match_atom(
        self, parent: Optional["SelectedEbuild"], atom: Atom, ebuild: Ebuild
    ) -> SelectedEbuild:
        for slot in self.selected_ebuild_slots:
            if atom.matches(self.selected_ebuild_slots[slot].ebuild):
                return self.selected_ebuild_slots[slot]

        # Check if slot has already been selected
        if ebuild.slot is not None and ebuild.slot in self.selected_ebuild_slots:
            # See if this atom matches the selected ebuild
            sel_ebuild = self.selected_ebuild_slots[ebuild.slot]
            if atom.matches(sel_ebuild.ebuild):
                return sel_ebuild  # We're good

            # See if we can change the selected ebuild to match this atom
            if sel_ebuild.change_within_slot(atom):
                return sel_ebuild  # Works!
            else:
                raise NotImplementedError("Cannot split a slot into multi-slot yet!")
        elif ebuild.slot is not None:
            # See if this atom matches any of the currently scheduled slots
            for key, sel_ebuild in self.selected_ebuild_slots.items():
                if atom.matches(sel_ebuild.ebuild):
                    return sel_ebuild

            # We need to create a new selected ebuild and add it here
            return self.add_atom(parent, atom, ebuild)

    def add_atom(
        self, parent: Optional["SelectedEbuild"], atom: Atom, ebuild: Ebuild
    ) -> SelectedEbuild:
        """
        Add a SelectedEbuild in its slot
        :param parent: parent package that triggered this
        :param atom: atom that selected with ebuild
        :param ebuild: ebuild selected by atom
        :return: SelectedEbuild generated from the atom+ebuild
        """

        sel_ebuild = SelectedEbuild(parent, atom, ebuild, self.resolve_session)
        self.selected_ebuild_slots[sel_ebuild.ebuild.slot] = sel_ebuild
        return sel_ebuild


class Emerge:
    portage: Portage
    selected_packages: Dict[str, InstallPackage]
    blocks: Dict[str, List[Atom]]
    resolve_session: "PackageResolutionSession"

    def __init__(self, resolve_session: "PackageResolutionSession"):
        self.portage = get_portage()
        self.selected_packages = {}
        self.blocks = {}
        self.resolve_session = resolve_session

    def add_block(self, _: Optional["SelectedEbuild"], atom: Atom):
        """
        Block ebuilds match this atom from being selected
        :param _: ebuild that selected this block
        :param atom: atom to block
        :return:
        """

        if atom.key not in self.blocks:
            self.blocks[atom.key] = []

        self.blocks[atom.key].append(atom)

    def select_atom(
        self, parent: Optional["SelectedEbuild"], atom: Atom
    ) -> SelectedEbuild:
        ebuild = self.portage.match_atom(atom)
        if ebuild is None:
            raise ResolutionException("No ebuild to match '%s' could be found" % atom)

        if ebuild.package_key in self.selected_packages:
            # Ebuild with this key has already been selected
            # See if we can match this to an existing slot
            install_pkg = self.selected_packages[ebuild.package_key]
            return install_pkg.match_atom(parent, atom, ebuild)
        else:
            pkg = InstallPackage(atom.key, self.resolve_session)
            self.selected_packages[pkg.key] = pkg
            return pkg.add_atom(parent, atom, ebuild)


class PackageResolutionSession:
    current_resolution: Set[SelectedEbuild]

    def __init__(self):
        self.current_resolution = set()

    def check_resolved(self, ebuild: SelectedEbuild) -> bool:
        return ebuild in self.current_resolution

    def add_to_session(self, ebuild):
        self.current_resolution.add(ebuild)

    def clear(self):
        self.current_resolution.clear()
