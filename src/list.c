#include "general.h"
#include "list.h"
#include "malloc.h"

List * listCreate(void) {
    List *l = (List *)calloc(1, sizeof(List));
    return l;
}

void listRelease(List *l) {
    /* Release nodes */
    LNode *current = l->head, *next;

    while (current != NULL) {
        next = current->next;

        if (l->release != NULL)
            l->release(current->value);
        free(current);

        current = next;
    }
    free(l);
}

List *listDup(List *l) {
    List *dup = (List *)calloc(1, sizeof(List));
    listSetCopyMethod(dup, l->copy);
    listSetEqlMethod(dup, l->equal);
    listSetRelMethod(dup, l->release);

    /* Node Duplicate */
    LNode *current;
    void *dup_val;
    for (current = l->head; current != NULL; current = current->next) {
        dup_val = l->copy(current->value);
        listAppend(dup, dup_val);
    }

    return dup;
}

_Bool listEqual(List *l, List *r) {
    if (l->length != r->length ||
        l->copy != r->copy) {
        return false;
    }

    LNode *l_node = l->head, *r_node = r->head;

    while (l_node != NULL && r_node != NULL) {
      if (! l->equal(l_node->value, r_node->value)) {
          return false;
      }
      l_node = l_node->next;
      r_node = r_node->next;
    }

    return true;
}

List * listAppend(List *l, void *value) {
    /* Create a new node */
    LNode *new_node = (LNode *)calloc(1, sizeof(LNode));
    new_node->value = value;

    /* Append to list */
    if (l->length == 0) {
        l->head = l->tail = new_node;
    } else {
        l->tail->next = new_node;
        new_node->prev = l->tail;
        l->tail = new_node;
    }

    l->length += 1;

    return l;
}

List * listPrepend(List *l, void *value) {
    /* Create a new node */
    LNode *new_node = (LNode *)calloc(1, sizeof(LNode));
    new_node->value = value;

    /* Prepend to list */
    if (l->length == 0) {
        l->head = l->tail = new_node;
    } else {
        l->head->prev = new_node;
        new_node->next = l->head;
        l->head = new_node;
    }

    l->length += 1;

    return l;
}

List *listDel(List *l, LNode *node) {
    // Suppose node must reside in  l
    if (node->prev && node->next) {
        node->next->prev = node->prev;
        node->prev->next = node->next;
    } else if (!node->prev && !node->next) {
        l->head = NULL;
        l->tail = NULL;
    } else if (!node->next) {
        l->head = node->next;
        l->head->prev = NULL;
    } else if (!node->prev) {
        l->tail = node->prev;
        l->tail->next = NULL;
    }

    if (l->release)
        l->release(node->value);
    free(node);

    l->length--;

    return l;
}

ListIter *listIter(List *l, int dir) {
    ListIter *iter = (ListIter *)calloc(1, sizeof(ListIter));
    iter->direction = dir;

    if (dir == 0) {
        iter->next = l->head;
    } else {
        iter->next = l->tail;
    }

    return iter;
}

LNode *listNext(ListIter *iter) {
    if (iter->next == NULL) {
        return NULL;
    }

    LNode *node = iter->next;

    /* iterate to next node */
    if (iter->direction == 0) {
        iter->next = iter->next->next;
    } else {
        iter->next = iter->next->prev;
    }

    return node;
}

LNode *listPrev(ListIter *iter) {
    if (iter->next == NULL) {
        return NULL;
    }

    LNode *node = iter->next;;

    /* iterate to prev node */
    if (iter->direction == 0) {
        iter->next = iter->next->prev;
    } else {
        iter->next = iter->next->next;
    }

    return node;
}

ListIter *listRewind(List *l, ListIter *iter) {
    int dir = iter->direction;

    if (dir == 0) {
        iter->next = l->head;
    } else {
        iter->next = l->tail;
    }
}
