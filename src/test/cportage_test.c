//
// Created by tumbar on 12/3/20.
//

#include <stdio.h>
#include <cportage/atom.h>
#include <cportage/language.h>
#include <cportage/module.h>
#include <setjmp.h>
#include <cmocka.h>
#include <ebuild.h>

#define CTEST(name) static void name(void** state)

CTEST(test_atom_full)
{
    Atom* atom = atom_parse("!!>=cat2/pkg3-2.2.34-r6:3/32::repository"
                            "[use_enable, -use_disable,"
                            "use_equal=, !use_opposite=,"
                            "!disable_if_off?, enable_if_on?]");
    assert_string_equal(atom->category, "cat2");
    assert_string_equal(atom->name, "pkg3");
    assert_string_equal(atom->version->full_version, "2.2.34-r6");
    assert_int_equal(atom->version->revision, 6);
    assert_string_equal(atom->repository, "repository");

    AtomFlag* current_flag = atom->useflags;

    assert_string_equal(current_flag->name, "use_enable");
    assert_int_equal(current_flag->option, ATOM_USE_ENABLE);
    current_flag = current_flag->next;

    assert_string_equal(current_flag->name, "use_disable");
    assert_int_equal(current_flag->option, ATOM_USE_DISABLE);
    current_flag = current_flag->next;

    assert_string_equal(current_flag->name, "use_equal");
    assert_int_equal(current_flag->option, ATOM_USE_EQUAL);
    current_flag = current_flag->next;

    assert_string_equal(current_flag->name, "use_opposite");
    assert_int_equal(current_flag->option, ATOM_USE_OPPOSITE);
    current_flag = current_flag->next;

    assert_string_equal(current_flag->name, "disable_if_off");
    assert_int_equal(current_flag->option, ATOM_USE_DISABLE_IF_OFF);
    current_flag = current_flag->next;

    assert_string_equal(current_flag->name, "enable_if_on");
    assert_int_equal(current_flag->option, ATOM_USE_ENABLE_IF_ON);
    current_flag = current_flag->next;

    assert_null(current_flag);
    assert_string_equal(atom->slot, "3");
    assert_string_equal(atom->sub_slot, "32");

    assert_int_equal(atom->range, ATOM_VERSION_GE);
    assert_int_equal(atom->blocks, ATOM_BLOCK_HARD);

    Py_DECREF(atom);
}

CTEST(test_ebuild_init)
{
    Ebuild* self = (Ebuild*) PyEbuild_new(&PyEbuildType, NULL, NULL);
    assert_int_equal(ebuild_init(self, "/var/db/repos/gentoo", "sys-devel", "gcc-9.3.0-r2"), 0);

    assert_int_equal(ebuild_metadata_init(self), 0);
    Py_DECREF(self);
}

const static struct CMUnitTest cportage_tests[] = {
        cmocka_unit_test(test_atom_full),
        cmocka_unit_test(test_ebuild_init),
};

int main(void)
{
    Portage* p;
    PyObject* module;

    Py_Initialize();
    module = PyInit_autogentoo_cportage();
    if (!module)
        return 1;

    p = (Portage*) PyPortage_new(&PyPortageType, NULL, NULL);
    PyPortage_init(p, NULL, NULL);
    PyCportage_Init(NULL, (PyObject* const*) &p, 1);

    int ret = cmocka_run_group_tests(cportage_tests, NULL, NULL);

    Py_DECREF(p);
    Py_DECREF(module);

    if (Py_FinalizeEx() != 0)
    {
        lerror("Failed to free Python memory!");
    }

    return ret;
}
