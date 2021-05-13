#include "ctest.h"
#include "dict.h"
#include "dictType.h"
#include <malloc.h>
#include <strings.h>

int test_hash(void *key) {
    return 0;
}

DictType test_dt = { test_hash,
                      NULL,
                      NULL,
                      free,
                      free };

CTEST_DATA(DictUT) {
  Dict *d;
};

CTEST_SETUP(DictUT) {
    data->d = dictCreate(&test_dt, 10);
}

CTEST_TEARDOWN(DictUT) {
    dictRelease(data->d);
}

CTEST2(DictUT, Instantiation) {
    // Do nothing
}

CTEST2(DictUT, Add) {
    Dict *d = data->d;

    dictAdd(d, strdup("K1"), strdup("123456"));
    ASSERT_EQUAL(1, dictTotal(d));

    DictEntry *entry = d->entries[dictHashing(d, "K1")];
    ASSERT_STR("K1", (char *)entry->key);
    ASSERT_STR("123456", (char *)entry->data);
}
