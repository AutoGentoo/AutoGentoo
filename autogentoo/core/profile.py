from typing import Tuple, Set, Dict
from portage import Portage
from cportage import Atom, UseFlag, Package


class Profile:
    """
    A profile will model a systems package tree.

    This class will keep track of use flags
    on the global and local level to allow the
    re-use on different machines.
    """

    parent: Portage
    name: str
    flags: Set[Tuple[Atom, UseFlag]]
    package_flags: Dict[Package, Set[Tuple[Atom, UseFlag]]]

    def __init__(self, parent: Portage, name: str):
        self.parent = parent
        self.name = name
        self.flags = set()
        self.package_flags = {}

    def add_flag(self, atom: Atom, flag: UseFlag):
        package = self.parent.default.get_package(atom.id)

        if package not in self.package_flags:
            self.package_flags[package] = set()
        self.package_flags[package].add((atom, flag))
        self.flags.add((atom, flag))

    def generate_build_configs(self):
        package_use = self.parent.get_config_path("package.use")
        # package_keywords = self.parent.get_config_path("package.accept_keywords")

        with package_use.open("w+") as f:
            # Group together package version query
            # To reduce the number of lines in package.use
            for package, queries in self.package_flags.items():
                # The atoms must be equivalent to
                # group together useflag queries
                atom_groups: Dict[Atom, Set[UseFlag]] = {}
                for atom, query in queries:
                    if atom not in atom_groups:
                        atom_groups[atom] = set()
                    atom_groups[atom].add(query)

                # Write the comment
                f.write("# Package %s\n" % package.key)
                for atom, useflags in atom_groups.items():
                    f.write(f"{atom} {' '.join([repr(x) for x in useflags])}\n")
                f.write("\n")
