#!/usr/bin/env python3
import os
import sys
import unittest
from typing import List

sys.path.append(os.path.join(os.path.dirname(__file__), "../"))
import cportage


class TestStringMethods(unittest.TestCase):
    def setUp(self) -> None:
        cportage.init(cportage.Portage())

    def test_atom_1(self):
        atom = cportage.Atom("cat/pkg-1.2.34")
        self.assertEqual(atom.category, "cat")
        self.assertEqual(atom.name, "pkg")
        self.assertEqual(atom.version.raw, "1.2.34")
        self.assertEqual(atom.useflags, None)

    def test_atom_2(self):
        atom = cportage.Atom("cat2/pkg3-2.2.34")
        self.assertEqual(repr(atom), "Atom<category=cat2, name=pkg3>")
        self.assertEqual(atom.category, "cat2")
        self.assertEqual(atom.name, "pkg3")
        self.assertEqual(atom.version.raw, "2.2.34")
        self.assertEqual(atom.useflags, None)

    def test_atom_use(self):
        atom = cportage.Atom("cat2/pkg3-2.2.34[flag1,-flag2]")
        print(atom.useflags)
        print(atom.useflags.next)
        print(list(atom.useflags))

    def test_parse_1(self):
        deps = cportage.Dependency("use1? ( cat2/pkg3-2.2.34 cat1/pkg2-2.2.34 )")

        deps_children: List[cportage.Dependency] = list(deps.children)
        self.assertEqual(deps_children[0].atom.category, "cat2")
        self.assertEqual(deps_children[0].atom.name, "pkg3")
        self.assertEqual(deps_children[1].atom.category, "cat1")
        self.assertEqual(deps_children[1].atom.name, "pkg2")

    def test_parse_2(self):
        pass


if __name__ == '__main__':
    unittest.main()
