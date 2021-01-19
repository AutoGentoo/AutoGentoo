//
// Created by tumbar on 12/3/20.
//

#include <cportage/language.h>
#include <stdio.h>
#include <cportage/atom.h>
#include <cportage/module.h>
#include <cportage/package.h>
#include <cmocka.h>
#include <ebuild.h>
#include <dirent.h>

struct atom_verify_proto {
    const char* category;
    const char* name;
    const char* full_version;
    atom_version_t version_select;
};

struct use_verify_proto {
    const char* use_name;
    use_operator_t operator;
};

static Dependency* dependency_verify_atom(Dependency* current, U32 count, struct atom_verify_proto verify[])
{
    for (U32 i = 0; i < count; i++)
    {
        assert_non_null(current->atom);
        assert_string_equal(current->atom->category, verify[i].category);
        assert_string_equal(current->atom->name, verify[i].name);

        if (verify[i].full_version)
        {
            assert_non_null(current->atom->version);
            assert_string_equal(current->atom->version->full_version, verify[i].full_version);
        }
        else
        {
            assert_null(current->atom->version);
        }

        assert_int_equal(current->atom->range, verify[i].version_select);
        current = current->next;
    }

    return current;
}

static Dependency* dependency_verify_use(Dependency* current, U32 count, struct use_verify_proto verify[])
{
    for (U32 i = 0; i < count; i++)
    {
        assert_null(current->atom);
        assert_int_equal(current->use_operator, verify[i].operator);
        if (current->use_condition)
            assert_string_equal(lut_get_key(global_portage->global_flags, current->use_condition), verify[i].use_name);
        current = current->next;
    }

    return current;
}

#define CTEST(name) static void name(void** state)

CTEST(test_atom_full)
{
    void* buffers = depend_allocate_buffers();
    Atom* atom = atom_parse(buffers,
                      "!!>=cat2/pkg3-2.2.34-r6:3/32::repository"
                            "[use_enable, -use_disable,"
                            "use_equal=, !use_opposite=,"
                            "!disable_if_off?, enable_if_on?]");
    depend_free_buffers(buffers);
    assert_non_null(atom);
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

CTEST(test_depend)
{
    void* buffers = depend_allocate_buffers();
    Dependency* deps = depend_parse(buffers, "use1? ( >=cat2/pkg3-2.2.34 cat1/pkg2-2.2.34 )");
    assert_non_null(deps);

    assert_string_equal(lut_get_key(global_portage->global_flags, deps->use_condition), "use1");
    assert_string_equal(deps->children->atom->name, "pkg3");
    assert_string_equal(deps->children->next->atom->name, "pkg2");

    Dependency* test_long = depend_parse(
            buffers,
            ">=app-xemacs/ada-1.16 >=app-xemacs/apel-1.36 "
            ">=app-xemacs/auctex-1.58 >=app-xemacs/bbdb-1.34 >=app-xemacs/build-1.18 "
            ">=app-xemacs/c-support-1.24 >=app-xemacs/calc-1.28 "
            ">=app-xemacs/calendar-1.42 >=app-xemacs/cc-mode-1.99 "
            ">=app-xemacs/cedet-common-1.03 >=app-xemacs/clearcase-1.12 >=app-xemacs/cogre-1.04 "
            ">=app-xemacs/cookie-1.17 >=app-xemacs/crisp-1.17 >=app-xemacs/debug-1.20 "
            ">=app-xemacs/dictionary-1.18 >=app-xemacs/dired-1.22 >=app-xemacs/docbookide-0.10 "
            ">=app-xemacs/easypg-1.05 >=app-xemacs/ecb-1.25 >=app-xemacs/ecrypto-0.23 "
            ">=app-xemacs/ede-1.07 >=app-xemacs/edebug-1.24 >=app-xemacs/ediff-1.84 "
            ">=app-xemacs/edit-utils-2.58 >=app-xemacs/edt-1.16 >=app-xemacs/efs-1.37 "
            ">=app-xemacs/eieio-1.10 >=app-xemacs/elib-1.15 >=app-xemacs/emerge-1.13 "
            ">=app-xemacs/erc-0.26 >=app-xemacs/escreen-1.03 >=app-xemacs/eshell-1.21 "
            ">=app-xemacs/eudc-1.43 >=app-xemacs/footnote-1.18 >=app-xemacs/forms-1.17 "
            ">=app-xemacs/fortran-modes-1.07 >=app-xemacs/frame-icon-1.13 >=app-xemacs/fsf-compat-1.24 "
            ">=app-xemacs/games-1.23 >=app-xemacs/general-docs-1.10 >=app-xemacs/gnats-1.19 "
            ">=app-xemacs/gnus-2.04 >=app-xemacs/haskell-mode-1.14 >=app-xemacs/hm-html-menus-1.26 "
            ">=app-xemacs/hyperbole-1.22 >=app-xemacs/ibuffer-1.12 >=app-xemacs/idlwave-1.34 "
            ">=app-xemacs/igrep-1.18 >=app-xemacs/ilisp-1.38 >=app-xemacs/jde-1.54 "
            ">=app-xemacs/mail-lib-1.84 >=app-xemacs/mailcrypt-2.16 >=app-xemacs/mew-1.22 "
            ">=app-xemacs/mh-e-1.35 >=app-xemacs/mine-1.18 >=app-xemacs/misc-games-1.25 "
            ">=app-xemacs/mmm-mode-1.08 >=app-xemacs/net-utils-1.61 >=app-xemacs/ocaml-0.10 "
            ">=app-xemacs/oo-browser-1.07 >=app-xemacs/os-utils-1.44 >=app-xemacs/pc-1.30 "
            ">=app-xemacs/pcl-cvs-1.73 >=app-xemacs/pcomplete-1.08 >=app-xemacs/perl-modes-1.16 "
            ">=app-xemacs/pgg-1.10 >=app-xemacs/prog-modes-2.33 >=app-xemacs/ps-print-1.14 "
            ">=app-xemacs/psgml-1.50 >=app-xemacs/psgml-dtds-1.05 >=app-xemacs/python-modes-1.16 "
            ">=app-xemacs/re-builder-1.07 >=app-xemacs/reftex-1.36 >=app-xemacs/riece-1.26 "
            ">=app-xemacs/rmail-1.16 >=app-xemacs/ruby-modes-1.07 >=app-xemacs/sasl-1.18 "
            ">=app-xemacs/scheme-1.22 >=app-xemacs/semantic-1.23 >=app-xemacs/sgml-1.13 "
            ">=app-xemacs/sh-script-1.26 >=app-xemacs/sieve-1.20 >=app-xemacs/slider-1.18 "
            ">=app-xemacs/sml-mode-0.14 >=app-xemacs/sounds-au-1.14 >=app-xemacs/sounds-wav-1.14 "
            ">=app-xemacs/speedbar-1.32 >=app-xemacs/strokes-1.12 >=app-xemacs/sun-1.19 "
            ">=app-xemacs/supercite-1.23 >=app-xemacs/texinfo-1.32 >=app-xemacs/text-modes-2.06 "
            ">=app-xemacs/textools-1.17 >=app-xemacs/time-1.17 >=app-xemacs/tm-1.42 "
            ">=app-xemacs/tooltalk-1.17 >=app-xemacs/tpu-1.16 >=app-xemacs/tramp-1.57 "
            ">=app-xemacs/vc-1.50 >=app-xemacs/vc-cc-1.24 >=app-xemacs/vhdl-1.24 "
            ">=app-xemacs/view-process-1.15 >=app-xemacs/viper-1.69 >=app-xemacs/vm-8.11 "
            ">=app-xemacs/w3-1.41 >=app-xemacs/x-symbol-1.16 >=app-xemacs/xemacs-base-2.46 "
            ">=app-xemacs/xemacs-devel-1.84 >=app-xemacs/xemacs-eterm-1.20 "
            ">=app-xemacs/xemacs-ispell-1.37 >=app-xemacs/xetla-1.04 >=app-xemacs/xlib-1.16 "
            ">=app-xemacs/xslide-1.11 >=app-xemacs/xslt-process-1.14 >=app-xemacs/xwem-1.26 "
            ">=app-xemacs/zenirc-1.18 mule? ( >=app-xemacs/edict-1.19 >=app-xemacs/egg-its-1.29 "
            ">=app-xemacs/latin-euro-standards-1.11 >=app-xemacs/latin-unity-1.23 "
            ">=app-xemacs/leim-1.38 >=app-xemacs/locale-1.30 >=app-xemacs/lookup-1.17 "
            ">=app-xemacs/mule-base-1.60 >=app-xemacs/mule-ucs-1.21 >=app-xemacs/skk-1.27 )");

    assert_non_null(test_long);
    depend_free_buffers(buffers);

    Py_DECREF(deps);
    Py_DECREF(test_long);
}

CTEST(test_ebuild_init)
{
    Ebuild* self = (Ebuild*) PyEbuild_new(&PyEbuildType, NULL, NULL);
    assert_non_null(self);
    assert_int_equal(ebuild_init(self, "data/test-repo",
                                 "data/test-repo/metadata",
                                 "sys-devel", "gcc-9.3.0-r1"), 0);
    assert_int_equal(ebuild_metadata_init(self), 0);

    assert_non_null(self->pdepend);
    assert_non_null(self->rdepend);
    assert_non_null(self->bdepend);
    assert_non_null(self->depend);

    struct atom_verify_proto verify_atom_1[] = {
            {"app-portage", "elt-patches", "20170815", ATOM_VERSION_GE},
            {"sys-devel", "bison", "1.875", ATOM_VERSION_GE},
            {"sys-devel", "flex", "2.5.4", ATOM_VERSION_GE},
    };
    struct atom_verify_proto verify_atom_2[] = {
            {"sys-devel", "gettext", NULL, ATOM_VERSION_ALL},
    };
    struct atom_verify_proto verify_atom_3[] = {
            {"dev-util", "dejagnu", "1.4.4", ATOM_VERSION_GE},
            {"sys-devel", "autogen", "5.5.4", ATOM_VERSION_GE},
    };

    struct use_verify_proto verify_use_1[] = {
            {"nls", USE_OP_ENABLE}
    };
    struct use_verify_proto verify_use_2[] = {
            {"test", USE_OP_ENABLE}
    };

    Dependency* current = self->bdepend;
    current = dependency_verify_atom(current, 3, verify_atom_1);
    assert_null(dependency_verify_atom(current->children, 1, verify_atom_2));
    current = dependency_verify_use(current, 1, verify_use_1);
    assert_null(dependency_verify_atom(current->children, 2, verify_atom_3));
    assert_null(dependency_verify_use(current, 1, verify_use_2));

    assert_int_equal(Py_REFCNT(self), 1);
    Py_DECREF(self);
}

CTEST(test_parse_all_metadata)
{
    /* Parse the entire repository */
    /* We can just free each ebuild immediately */

    struct dirent* dp;
    struct dirent* dp2;
    DIR* dfd;
    DIR* dfd2;

    const char* cache_path = "data/cportage-repo";
    assert_non_null(dfd = opendir(cache_path));

    char path1[PATH_MAX];
    U32 i = 0;
    while ((dp = readdir(dfd)) != NULL)
    {
        struct stat stbuf;
        sprintf(path1, "%s/%s", cache_path, dp->d_name);
        assert_int_not_equal(stat(path1, &stbuf), -1);

        if (*dp->d_name == '.')
            continue;

        if (S_ISDIR(stbuf.st_mode))
        {
            const char* category = dp->d_name;
            assert_non_null(dfd2 = opendir(path1));

            while ((dp2 = readdir(dfd2)) != NULL)
            {
                if (*dp2->d_name == '.' || strcmp(dp2->d_name, "Manifest.gz") == 0)
                    continue;

                const char* name_and_value = dp2->d_name;

                Ebuild* self = (Ebuild*) PyEbuild_new(&PyEbuildType, NULL, NULL);

                /* Don't search for ebuild file, just initialize the metadata */
                errno = 0;
                assert_int_equal(ebuild_init(self, NULL, cache_path, category, name_and_value), 0);

#ifndef __APPLE__ // Man, is this slow on Mac OSX. Probably because its allocating the RE2 buffers to swap
                assert_int_equal(ebuild_metadata_init(self), 0);
#endif
                assert_int_equal(errno, 0);
                Py_XDECREF(self);
                i++;
            }

            closedir(dfd2);
        }
    }

    closedir(dfd);
    assert_int_equal(i, 30260);
}

CTEST(test_parse_invalid)
{
    void* buffers = depend_allocate_buffers();
    assert_null(atom_parse(buffers, "package-name-not-atom"));
    assert_null(required_use_parse(buffers, " ?? ( use_flag "));
    assert_null(depend_parse(buffers, "use? "));
    depend_free_buffers(buffers);
}

CTEST(test_package_key)
{
    PyObject* args = PyTuple_New(1);
    PyObject* pkg_key = PyUnicode_FromString("sys-devel/gcc");
    Py_INCREF(pkg_key);
    PyTuple_SetItem(args, 0, pkg_key);
    PyObject* pkg = PyObject_CallObject((PyObject*) &PyPackageType, args);

    assert_non_null(pkg);

    assert_int_equal(Py_REFCNT(pkg), 1);
    PyPortage_add_package(global_portage, &pkg, 1);
    assert_int_equal(Py_REFCNT(pkg), 2);

    PyObject* pkg_id = PyLong_FromUnsignedLongLong(((Package*)pkg)->package_id);
    PyObject* pkg_2 = PyPortage_get_package(global_portage, &pkg_id, 1);
    assert_int_equal(Py_REFCNT(pkg), 3);

    assert_ptr_equal(pkg, pkg_2);

    Py_DECREF(pkg_id);
    Py_DECREF(args);
    Py_DECREF(pkg_key);
    Py_DECREF(pkg_2);
    Py_DECREF(pkg);

    // Still held by the LUT
    assert_int_equal(Py_REFCNT(pkg), 1);
}

CTEST(test_misc_required_use)
{
    const char* test_strings[] = {
            "emacs? ( gtk ) !curl? ( !gtk )",
            "libunwind? ( libcxxabi )",
            "libfuzzer? ( || ( sanitize xray ) )",
    };

    void* buffers = required_use_allocate_buffers();
    for (int i = 0; i < sizeof(test_strings) / sizeof(test_strings[0]); i++)
    {
        RequiredUse* parsed = required_use_parse(buffers, test_strings[i]);
        assert_non_null(parsed);
        Py_XDECREF(parsed);
    }
    required_use_free_buffers(buffers);
}

const static struct CMUnitTest cportage_tests[] = {
        cmocka_unit_test(test_misc_required_use),
        cmocka_unit_test(test_depend),
        cmocka_unit_test(test_atom_full),
        cmocka_unit_test(test_ebuild_init),
        cmocka_unit_test(test_parse_all_metadata),
        cmocka_unit_test(test_parse_invalid),
        cmocka_unit_test(test_package_key),
};

int main(void)
{
    Portage* p;
    PyObject* module;

    Py_Initialize();
    module = PyInit_autogentoo_cportage();
    assert_non_null(module);

    p = (Portage*) PyPortage_new(&PyPortageType, NULL, NULL);
    PyPortage_init(p, NULL, NULL);
    PyCportage_Init(NULL, (PyObject* const*) &p, 1);

    int ret = cmocka_run_group_tests(cportage_tests, NULL, NULL);

    Py_DECREF(p);
    Py_DECREF(module);

    Py_FinalizeEx();

    return ret;
}
