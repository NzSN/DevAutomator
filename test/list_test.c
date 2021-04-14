#include "ctest.h"
#include "list.h"

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

CTEST2(ListT, AppendAndPrepend) {
    List *l = data->l;

    /* 100 is the first element of
     * the list */
    listAppend(l, (void *)100);

    /* Then we append 101 into the list
     * and 101 should be the next of 100 */
    listAppend(l, (void *)101);

    /* At the last prepend 99 to list */
    listAppend(l, (void *)99);

    /* Verify */
    unsigned long number;
    int right_order[3] = {99, 100, 101};
    ListIter *lIter = listIter(l);

    for (number = (unsigned long)listNext(lIter); (void *)number != NULL;
         number = (unsigned long)listNext(lIter)) {

        ASSERT_EQUAL(right_order[number-99], number);
    }
}
