#include "general.h"

#ifndef _DICT_H_
#define _DICT_H_

typedef struct DictType {
    int (*hash)(void *);
    _Bool (*equal)(void *, void *);
    void * (*dup)(void *);
    void (*keyRelease)(void *);
    void (*dataRelease)(void *);
} DictType;

typedef struct DictEntry {
    void *key;
    void *data;
    struct DictEntry *next;
} DictEntry;

typedef struct Dict {
  DictType *dt;
  // Number of entry
  int k;
  DictEntry **entries;
  int total;
} Dict;

/* Dict functions as macros */
#define dictTotal(d) ((d)->total)
#define dictNumOfEntries(d) ((d)->k)
#define dictGetEntry(d, idx) ((d)->entries[(idx)])
#define dictSetEntry(d, idx, ent) ((d)->entries[idx] = ent)
#define dictType(d) ((d)->dt)
#define dictHashing(d, k) ((d)->dt->hash(k))

/* Prototypes */
Dict * dictCreate(DictType *, int k);
void dictRelease(Dict *d);
void dictAdd(Dict *d, void *key, void *data);
void dictReplace(Dict *d, void *key, void *data);
void dictDelete(Dict *d, void *key);

#endif /* _DICT_H_ */
