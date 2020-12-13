//
// Created by atuser on 12/12/20.
//

#include <Python.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include <hacksaw/hacksaw.h>

#define CTEST(name) static void name(void** state)

CTEST(test_map)
{
    RefObject* ref = malloc(sizeof(RefObject));
    ref->reference_count = 1;
    ref->free = free;

    Map* map = map_new(32, 0.8);
    map_insert(map, "key", ref);
    assert_ptr_equal(map_get(map, "key"), ref);
    assert_int_equal(ref->reference_count, 2);
    OBJECT_DECREF(ref);
    assert_int_equal(ref->reference_count, 1);

    OBJECT_FREE(map);
}

CTEST(test_lut)
{
    LUT* lut = lut_new(32);
    U64 data = 0xfafbef0adccccccc;

    U64 id = lut_insert(lut, "key", data, 0);
    assert_int_equal(data, lut_get(lut, id));

    lut_flag_t flags;
    assert_int_equal(data, lut_get(lut, lut_get_id(lut, "key", &flags)));
    assert_int_equal(flags, LUT_FLAG_EXISTS);

    OBJECT_FREE(lut);
}

const static struct CMUnitTest hacksaw_tests[] = {
        cmocka_unit_test(test_map),
        cmocka_unit_test(test_lut),
};

int main(void) {
    Py_Initialize();

    int ret = cmocka_run_group_tests(hacksaw_tests, NULL, NULL);

    assert_int_equal(Py_FinalizeEx(), 0);
    return ret;
}
