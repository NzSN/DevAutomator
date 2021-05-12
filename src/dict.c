#include "dict.h"
#include "malloc.h"

/* Private Prototypes */
void dictEntryRelease(DictEntry *ent, DictType *dt);


Dict *dictCreate(DictType *dt, int k) {
    Dict *d = (Dict *)calloc(1, sizeof(Dict));
    d->dt = dt;
    d->k = k;
    d->entries = (DictEntry **)calloc(k, sizeof(DictEntry*));
    return d;
}

/* DictType is always static so need to
 * free it */
void dictRelease(Dict *d) {
    DictEntry *entry;
    int k = dictTotal(d);

    if (d == NULL) {
        return;
    }

    for (int i = 0; i < k; i++) {
        entry = dictGetEntry(d, i);

        if (entry == NULL) {
            continue;
        }

        dictEntryRelease(entry, dictType(d));
    }

    free(d);
}

void dictEntryRelease(DictEntry *ent, DictType *dt) {
    DictEntry *current = ent, *next;

    while (current != NULL) {
        next = current->next;

        dt->keyRelease(current->key);
        dt->dataRelease(current->data);

        free(current);
        current = next;
    }
}

void dictAdd(Dict *d, void *key, void *data) {
    int idx = dictHashing(d, key);

    DictEntry *entry = dictGetEntry(d, idx);
    if (entry == NULL) {

    }
}

void dictReplace(Dict *d, void *key, void *data) {

}

void dictDelete(Dict *d, void *key) {

}
