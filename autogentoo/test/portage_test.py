#!/usr/bin/env python3
import unittest

from autogentoo import cportage
from autogentoo.cportage import Package, Ebuild, Dependency, Atom
from autogentoo.portage import (
    resolve_all,
    Emerge,
    emerge_init,
    RequiredUseException,
    PackageResolutionSession,
)


class PortageUnitTests(unittest.TestCase):
    portage: cportage.Portage
    session: PackageResolutionSession

    def setUp(self) -> None:
        self.portage = cportage.Portage()
        cportage.init(self.portage)

        self.session = PackageResolutionSession()
        self.emerge = Emerge(self.session)
        emerge_init(self.emerge)

    def test_resolve_not_found(self):
        pkg = Package("sys-devel/gcc")
        pkg.add_ebuild(
            Ebuild(None, "data/test-repo/metadata", "sys-devel", "gcc-9.3.0-r1")
        )

        self.portage.add_package(pkg)

        layer_one = []

        # We have not initialized any other ebuild
        # When we will see an error when we try to resolve
        # the first atom, '>=app-portage/elt-patches-20170815'
        with self.assertRaises(KeyError):
            for x in resolve_all(None, Dependency(">=sys-devel/gcc-9.3.0")):
                layer_one.append(x.get_resolved())

    def test_resolve_gcc(self):
        ebuild_n = self.portage.initialize_repository(None, "data/cportage-repo")
        self.assertEqual(ebuild_n, 30260)

        request_atom = Dependency("sys-devel/gcc")

        while not self.emerge.finished():
            self.emerge.resolve_session.clear()
            try:
                for x in resolve_all(None, request_atom):
                    try:
                        resolved = x.get_resolved()
                        print(resolved, flush=True)
                    except RequiredUseException as r_use:
                        if r_use.suggestion is None:
                            raise r_use
                        else:
                            r_use.selected_by.apply_suggestion(r_use.suggestion)
            except RequiredUseException as e:
                suggestion = e.suggestion
                if suggestion is None:
                    raise e
                else:
                    e.selected_by.apply_suggestion(suggestion)


if __name__ == "__main__":
    unittest.main()
