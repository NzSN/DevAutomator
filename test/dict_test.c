#include "ctest.h"
#include "dict.h"
#include "dictType.h"
#include <malloc.h>
#include <string.h>

int test_hash(void *key) {
    int total = 0;
    char *s = key, c = *s;

    while (c != '\0') {
        c = *s;
        total += (short)*s;
        s++;
    }

    return total;
}

_Bool equal(void *l, void *r) {
    return !strcmp(l, r);
}

DictType test_dt = { test_hash,
                      equal,
                      strdup,
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
    dictAdd(d, strdup("K2"), strdup("1234567"));
    ASSERT_EQUAL(2, dictTotal(d));

    DictEntry *entry = d->entries[dictHashing(d, "K1")];
    ASSERT_STR("K1", (char *)entry->key);
    ASSERT_STR("123456", (char *)entry->data);

    entry = d->entries[dictHashing(d, "K2")];
    ASSERT_STR("K2", (char *)entry->key);
    ASSERT_STR("1234567", (char *)entry->data);

}

CTEST2(DictUT, Delete) {
    Dict *d = data->d;

    dictAdd(d, strdup("K1"), strdup("123456"));
    dictAdd(d, strdup("K2"), strdup("1234567"));

    dictDelete(d, "K1");

    /* Verify K1 is deleted
     * and K2 is not
     */
    ASSERT_EQUAL(1, dictTotal(d));
    DictEntry *entry = dictGetEntry(d, dictHashing(d, "K1"));
    ASSERT_EQUAL(NULL, entry);

    entry = dictGetEntry(d, dictHashing(d, "K2"));
    ASSERT_STR("K2", (char *)entry->key);
    ASSERT_STR("1234567", (char *)entry->data);

    dictDelete(d, "K2");

    /* Verify */
    ASSERT_EQUAL(0, dictTotal(d));
    entry = dictGetEntry(d, dictHashing(d, "K2"));
    ASSERT_EQUAL(NULL, entry);
}

CTEST2(DictUT, Replace) {
    Dict *d = data->d;

    dictAdd(d, strdup("K1"), strdup("123456"));
    dictReplace(d, "K1", strdup("654321"));

    /* Verify */
    void *val = dictGet(d, "K1");
    ASSERT_STR("654321", val);
}

CTEST2(DictUT, Conflict_Add) {
    Dict *d = data->d;

    dictAdd(d, strdup("123"), strdup("123456"));
    dictAdd(d, strdup("132"), strdup("123456"));
    dictAdd(d, strdup("321"), strdup("123456"));
}

CTEST2(DictUT, Conflict_Get) {
    Dict *d = data->d;

    dictAdd(d, strdup("123"), strdup("123456"));
    dictAdd(d, strdup("132"), strdup("123457"));
    dictAdd(d, strdup("321"), strdup("123458"));

    void *val = dictGet(d, "123");
    ASSERT_STR("123456", val);

    val = dictGet(d, "132");
    ASSERT_STR("123457", val);

    val = dictGet(d, "321");
    ASSERT_STR("123458", val);
}

CTEST2(DictUT, Conflict_IsExists) {
    Dict *d = data->d;

    dictAdd(d, strdup("123"), strdup("123456"));
    dictAdd(d, strdup("132"), strdup("123457"));

    ASSERT_EQUAL(true, dictIsExists(d, "123"));
    ASSERT_EQUAL(true, dictIsExists(d, "132"));
    ASSERT_EQUAL(false, dictIsExists(d, "133"));
}
