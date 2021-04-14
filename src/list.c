#include "general.h"
#include "list.h"
#include "malloc.h"

List * listCreate(void) {
    List *l = (List *)calloc(1, sizeof(List));
    return l;
}

void *listRelease(List *l) {
    return NULL;
}

List *listDup(List *l) {
    return NULL;
}

List *listEqual(List *l) {
    return NULL;
}

List * ListAppend(List *l, void *value) {
    return l;
}

List * listPrepend(List *l, void *value) {
    return l;
}

ListIter *listIter(List *l) {
    return NULL;
}

void *listNext(ListIter *) {
    return NULL;
}

void *listPrev(ListIter *) {
    return NULL;
}
