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
    void * (*copy)(const void *);
    void (*release)(void *);
    _Bool (*equal)(const void *, const void *);
    int length;
} List;

typedef struct ListIter {
    LNode *next;
    int direction;
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
void   listRelease(List *);
List * listDup(List *);
_Bool  listEqual(List *, List *);
List * listAppend(List *, void *value);
List * listPrepend(List *, void *value);
List * listDel(List *, LNode *node);
ListIter * listIter(List *, int dir);
LNode * listPrev(ListIter *);
LNode * listNext(ListIter *);
ListIter * listRewind(List *, ListIter *);



#endif /* _LIST_H_ */
