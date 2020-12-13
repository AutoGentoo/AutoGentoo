#!/usr/bin/env python3
import unittest

from autogentoo import cportage
from autogentoo.cportage import Package, Ebuild, Dependency
from autogentoo.portage import resolve_all, Emerge, emerge_init


class PortageUnitTests(unittest.TestCase):
    portage: cportage.Portage

    def setUp(self) -> None:
        self.portage = cportage.Portage()
        cportage.init(self.portage)

        self.emerge = Emerge()
        emerge_init(self.emerge)

    def test_parse(self):
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


if __name__ == "__main__":
    unittest.main()
