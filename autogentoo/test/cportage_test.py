#!/usr/bin/env python3
import unittest
from typing import List

from autogentoo import cportage
from autogentoo.cportage import UseOperatorT, Atom


class CPortageUnitTests(unittest.TestCase):
    portage: cportage.Portage

    def setUp(self) -> None:
        self.portage = cportage.Portage()
        cportage.init(self.portage)

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
        flags: List[cportage.AtomFlag] = list(atom.useflags)
        self.assertEqual(flags[0].name, "flag1")
        self.assertEqual(flags[1].name, "flag2")

    def test_atom_slot(self):
        atom = cportage.Atom("cat2/pkg3-2.2.34:3")
        self.assertEqual(atom.slot, "3")

        atom = cportage.Atom("cat2/pkg3-2.2.34:83/5")
        self.assertEqual(atom.slot, "83")
        self.assertEqual(atom.sub_slot, "5")

    def test_parse_1(self):
        deps = cportage.Dependency("use1? ( cat2/pkg3-2.2.34 cat1/pkg2-2.2.34 )")

        deps_children: List[cportage.Dependency] = list(deps.children)
        self.assertEqual(deps_children[0].atom.category, "cat2")
        self.assertEqual(deps_children[0].atom.name, "pkg3")
        self.assertEqual(deps_children[1].atom.category, "cat1")
        self.assertEqual(deps_children[1].atom.name, "pkg2")

    def test_parse_2(self):
        deps = cportage.Dependency("use1? ( >=cat2/pkg3-2.2.34 cat1/pkg2-2.2.34 )")
        self.assertEqual(deps.children.atom.range, cportage.AtomVersionT.GE)
        self.assertEqual(deps.children.next.atom.range, cportage.AtomVersionT.ALL)

    def test_readonly(self):
        atom = cportage.Atom("cat2/pkg3")
        with self.assertRaises(AttributeError):
            atom.id = 0

    def test_id_match(self):
        atom1 = cportage.Atom("cat/pkg-1.2.34")
        atom2 = cportage.Atom("cat/pkg")

        self.assertEqual(atom1.id, atom2.id)

    def test_version_compare_1(self):
        version_1 = cportage.AtomVersion("1.0.0")
        version_2 = cportage.AtomVersion("1.2.0")
        self.assertGreater(version_2, version_1)
        self.assertEqual(version_1, version_1)

    def test_version_compare_2(self):
        version_1 = cportage.AtomVersion("1.3.0")
        version_2 = cportage.AtomVersion("1.2.9")
        self.assertLess(version_2, version_1)

    def test_version_revision(self):
        atom1 = cportage.Atom("cat2/pkg3-2.2.34-r1")
        atom2 = cportage.Atom("cat2/pkg3-2.2.34-r2")

        self.assertGreater(atom2.version, atom1.version)

    def test_ebuild_1(self):
        ebuild = cportage.Ebuild(
            None, "data/test-repo/metadata", "sys-devel", "gcc-9.3.0-r1"
        )
        self.assertEqual(ebuild.category, "sys-devel")
        self.assertEqual(ebuild.name, "gcc")
        self.assertEqual(ebuild.package_key, "sys-devel/gcc")
        self.assertEqual(ebuild.key, "sys-devel/gcc-9.3.0-r1")
        self.assertEqual(ebuild.version, cportage.AtomVersion("9.3.0-r1"))

        self.assertEqual(ebuild.ebuild, None)
        # self.assertTrue(ebuild.ebuild.endswith("sys-devel/gcc/gcc-9.3.0-r1.ebuild"))
        self.assertTrue(ebuild.cache_file.endswith("sys-devel/gcc-9.3.0-r1"))
        # self.assertFalse(".." in ebuild.ebuild)  # Make sure the path was expanded
        self.assertFalse(".." in ebuild.cache_file)  # Make sure the path was expanded

    def test_init_error(self):
        with self.assertRaises(TypeError):
            cportage.init(None)

    def test_package(self):
        p = cportage.Package("sys-devel/gcc")
        self.portage.add_package(p)

        pkg: cportage.Package = self.portage.get_package(p.package_id)
        ebuild = cportage.Ebuild(
            None, "data/test-repo/metadata", "sys-devel", "gcc-9.3.0-r1"
        )
        pkg.add_ebuild(ebuild)

        # Can't add the same ebuild twice
        with self.assertRaises(RuntimeError):
            pkg.add_ebuild(ebuild)

        self.assertEqual(p, pkg)

    def test_required_use_double(self):
        r_use = cportage.RequiredUse("|| ( sna uxa )")
        self.assertEqual(UseOperatorT.LEAST_ONE, UseOperatorT(r_use.operator))
        self.assertEqual("sna", r_use.depend.name)
        self.assertEqual("uxa", r_use.depend.next.name)
        self.assertIsNone(r_use.depend.next.next)

    def test_required_use_single(self):
        r_use = cportage.RequiredUse("|| ( sna )")
        self.assertEqual(UseOperatorT.LEAST_ONE, UseOperatorT(r_use.operator))
        self.assertEqual("sna", r_use.depend.name)
        self.assertIsNone(r_use.depend.next)

    def test_required_use_simple(self):
        r_use = cportage.RequiredUse("sna")
        self.assertEqual("sna", r_use.name)

    def test_required_use_error(self):
        with self.assertRaises(RuntimeError):
            cportage.RequiredUse(" ?? ")

    def test_repository_init(self):
        ebuild_n = self.portage.initialize_repository(None, "data/cportage-repo")
        self.assertEqual(ebuild_n, 30260)

        self.assertIsNotNone(self.portage.match_atom(Atom("dev-vcs/cvs")))
        self.assertIsNotNone(self.portage.match_atom(Atom("sys-libs/zlib")))

        self.assertIsNotNone(self.portage.match_atom(Atom("dev-vcs/git-9.9.2")))
        self.assertIsNotNone(self.portage.match_atom(Atom("dev-vcs/git-merge-changelog")))


if __name__ == "__main__":
    unittest.main()
