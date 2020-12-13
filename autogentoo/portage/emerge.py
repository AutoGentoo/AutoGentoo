from typing import Optional, Dict

from autogentoo.cportage import Portage, get_portage, Atom, Ebuild
from autogentoo.portage import SelectedEbuild, ResolutionException


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


__emerge_session__: Optional[Emerge] = None


def emerge_init(emerge: Emerge):
    global __emerge_session__
    __emerge_session__ = emerge


def emerge_session() -> Emerge:
    global __emerge_session__
    if __emerge_session__ is None:
        raise RuntimeError("emerge_init() must be called before emerge_session() is called")

    return __emerge_session__
