#include "ctest.h"
#include "list.h"
#include <malloc.h>
#include <string.h>

/* To check tthat is list able to Instantial  */
CTEST_DATA(ListT) {
    List *l;
};

CTEST_SETUP(ListT) {
    data->l = listCreate();
}

CTEST_TEARDOWN(ListT) {
    listRelease(data->l);
}

CTEST2(ListT, CreateAndRelease) {
    /* These mems should be release correctly
     * able to check with valgrind */
    void *mem;
    for (int i = 0; i < 100; i++) {
        mem = (char *)malloc(100);
        listAppend(data->l, mem);
    }
    listSetRelMethod(data->l, free);
}

CTEST2(ListT, AppendAndPrepend) {
    List *l = data->l;

    /* 100 is the first element of
     * the list */
    listAppend(l, (void *)100);

    /* Then we append 101 into the list
     * and 101 should be the next of 100 */
    listAppend(l, (void *)101);

    /* At the last prepend 99 to list */
    listPrepend(l, (void *)99);

    /* Verify */
    LNode *node;
    int right_order[3] = {99, 100, 101}, i = 0;
    ListIter *lIter = listIter(l, 0);

    for (node = listNext(lIter); node != NULL;
         node = listNext(lIter)) {

        ASSERT_EQUAL(right_order[i], (unsigned long)(node->value));
        i += 1;
    }

    free(lIter);
}

_Bool strcmp_1(const char *l, const char *r) {
    int ret = strcmp(l, r);

    return !ret;
}

CTEST2(ListT, DuplicationAndEqual) {
    List *l = data->l;
    listSetCopyMethod(l, strdup);
    listSetEqlMethod(l, strcmp_1);
    listSetRelMethod(l, free);

    /* Add data to list */
    char *str;
    for (int i = 0; i < 100; i++) {
        str = strdup("123456");
        listAppend(l, str);
    }

    /* Create dup */
    List *dup_l = listDup(l);

    /* Verify */
    ASSERT_TRUE(listEqual(l, dup_l));

    listRelease(dup_l);
}

CTEST2(ListT, DeleteNode) {
    List *l = data->l;
    ListIter *iter;
    listSetRelMethod(l, free);

    /* Only one node */
    listAppend(l, strdup("123"));

    iter = listIter(l, 0);
    listDel(l, listNext(iter));
    free(iter);

    ASSERT_EQUAL(0, listLength(l));

    // Need generic algorithms to do more
    // tests.
}

CTEST2(ListT, Append_Performance) {
    List *l = data->l;

    for (int i = 0; i < 1000000; i++) {
        listAppend(l, (unsigned long)1);
    }
}
