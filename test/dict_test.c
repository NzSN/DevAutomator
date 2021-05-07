#include "ctest.h"
#include "dict.h"
#include "dictType.h"

CTEST_DATA(DictUT) {
    Dict *d;
};

CTEST_SETUP(DictUT) {
    data->d = dictCreate(NULL, 10);
}

CTEST_TEARDOWN(DictUT) {
    dictRelease(data->d);
}

CTEST2(DictUT, Instantiation) {
    // Do nothing
}

CTEST2(DictUT, Add) {
    Dict *d = data->d;

    dictAdd(d, "K1", "123456");

    ASSERT_EQUAL(1, dictTotal(d));

    DictEntry *entry = &d->entries[dictHash(d, "K1")];
    ASSERT_EQUAL("K1", (char *)entry->key);
    ASSERT_EQUAL("123456", (char *)entry->data);
}
