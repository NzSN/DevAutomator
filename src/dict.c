#include "dict.h"
#include "malloc.h"

/* Private Prototypes */
DictEntry * dictEntryFind(DictEntry *ent, void *key, DictType *dt);
void dictEntryDelete(DictEntry *ent, DictType *dt);
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

void dictAdd(Dict *d, void *key, void *data) {
    int idx = dictHashing(d, key);

    DictEntry *entry = dictGetEntry(d, idx),
        *new_entry = dictEntryCreate(key, data);
    if (entry == NULL) {
        dictSetEntry(d, idx, new_entry);
    } else {
        /* Hash conflict */
        new_entry->next = entry;
        entry->prev = new_entry;
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
    int idx = dictHashing(d, key);

    DictEntry *entry = dictGetEntry(d, idx);
    entry = dictEntryFind(entry, key, dictType(d));

    if (dictType(d)->dataRelease)
        dictType(d)->dataRelease(entry->data);

    entry->data = data;
}

void dictDelete(Dict *d, void *key) {
    int idx = dictHashing(d, key);

    DictEntry *entry = dictGetEntry(d, idx);
    if (entry == NULL)
        return;

    entry = dictEntryFind(entry, key, dictType(d));
    dictEntryDelete(entry, dictType(d));
    dictSetEntry(d, idx, NULL);
    d->total--;
}

void *dictGet(Dict *d, void *key) {
    int idx = dictHashing(d, key);

    DictEntry *entry = dictGetEntry(d, idx);
    if (entry == NULL)
        return NULL;

    entry = dictEntryFind(entry, key, dictType(d));
    if (entry == NULL)
        return NULL;

    return entry->data;
}

_Bool dictIsExists(Dict *d, void *key) {
    void *data = dictGet(d, key);
    return data != NULL;
}

/*****************************************************************************/
/*                          Procedures of DictEntry                          */
/*****************************************************************************/
void dictEntryDelete(DictEntry *ent, DictType *dt) {
  if (ent->prev != NULL) {
      ent->prev->next = ent->next;
  }
  if (ent->next != NULL) {
      ent->next->prev = ent->prev;
  }

  if (dt->keyRelease)
      dt->keyRelease(ent->key);
  if (dt->dataRelease)
      dt->dataRelease(ent->data);
  free(ent);
}

DictEntry * dictEntryFind(DictEntry *ent, void *key, DictType *dt) {
    DictEntry *current = ent;

    while (current != NULL) {
        if (dt->equal(key, current->key)) {
            return current;
        }
        current = current->next;
    }
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
