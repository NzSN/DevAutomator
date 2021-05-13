#include "dict.h"
#include "malloc.h"

/* Private Prototypes */
void dictEntryRelease(DictEntry *ent, DictType *dt);
DictEntry * dictEntryCreate(void *key, void *data);


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
    int k = dictNumOfEntries(d);

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

    free(d->entries);
    free(d);
}

void dictEntryRelease(DictEntry *ent, DictType *dt) {
    DictEntry *current = ent, *next;

    while (current != NULL) {
        next = current->next;

        if (dt->keyRelease)
            dt->keyRelease(current->key);
        if (dt->dataRelease)
            dt->dataRelease(current->data);

        free(current);
        current = next;
    }
}

void dictAdd(Dict *d, void *key, void *data) {
    int idx = dictHashing(d, key);

    DictEntry *entry = dictGetEntry(d, idx),
        *new_entry = dictEntryCreate(key, data);
    if (entry == NULL) {
        dictSetEntry(d, idx, new_entry);
    } else {
        /* Hash conflict */
        new_entry->next = entry;
        dictSetEntry(d, idx, new_entry);
    }

    d->total++;
}

DictEntry * dictEntryCreate(void *key, void *data) {
    DictEntry *entry = (DictEntry *)calloc(1, sizeof(DictEntry));

    entry->key = key;
    entry->data = data;

    return entry;
}

void dictReplace(Dict *d, void *key, void *data) {

}

void dictDelete(Dict *d, void *key) {

}
