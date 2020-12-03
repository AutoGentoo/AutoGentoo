#!/usr/bin/env python3
import os
import sys
import unittest
sys.path.append(os.path.join(os.path.dirname(__file__), "../"))
import cportage


class TestStringMethods(unittest.TestCase):
    def setUp(self) -> None:
        cportage.init(cportage.Portage())

    def test_atom_1(self):
        atom = cportage.Atom("cat/pkg-1.2.34")
        self.assertEqual(repr(atom), "Atom<category=cat, name=pkg>")
        self.assertEqual(repr(atom.version), "AtomVersion<1.2.34>")

    def test_atom_2(self):
        atom = cportage.Atom("cat2/pkg3-2.2.34")
        self.assertEqual(repr(atom), "Atom<category=cat2, name=pkg3>")
        self.assertEqual(repr(atom.version), "AtomVersion<2.2.34>")

    def test_parse_1(self):
        deps = cportage.Dependency.parse("use1? ( cat2/pkg3-2.2.34 cat1/pkg2-2.2.34 )")
        print(deps.children)

        for dep in deps.children:
            print(dep, dep.atom, dep.atom.id)

    def tearDown(self) -> None:
        cportage.close()

if __name__ == '__main__':
    unittest.main()
