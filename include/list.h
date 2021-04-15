#include "general.h"

#ifndef _LIST_H_
#define _LIST_H_

/* Definition */
typedef struct LNode {
    struct LNode *prev;
    struct LNode *next;
    void *value;
} LNode;

typedef struct List {
    LNode *head;
    LNode *tail;
    struct list * (*copy)(struct List *);
    void (*release)(struct List *);
    _Bool (*equal)(struct List *, struct List *);
    int length;
} List;

typedef struct ListIter {
    LNode *current;
    LNode *head;
    LNode *tail;
} ListIter;

/* List functions  s macros */
#define listFirst(l) ((l)->head)
#define listTail(l) ((l)->tail)
#define listPrevNode(ln) ((ln)->prev)
#define listNextNode(ln) ((ln)->next)
#define listLength(l) ((l)->length)
#define listSetCopyMethod(l,m) ((l)->copy = m)
#define listSetRelMethod(l,m) ((l)->release = m)
#define listSetEqlMethod(l,m) ((l)->equal = m)

/* Prototypes */
List * listCreate(void);
void * listRelease(List *);
List * listDup(List *);
List * listEqual(List *, List *);
List * listAppend(List *, void *value);
List * listPrepend(List *, void *value);
ListIter * listIter(List *);
void * listPrev(ListIter *);
void * listNext(ListIter *);



#endif /* _LIST_H_ */
