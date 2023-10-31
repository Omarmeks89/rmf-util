#include <stdlib.h>

#include "fifo.h"

struct fifo_node {
    int id;
    unsigned long hash;
    void *fdata;
    struct fifo_node *prev, *next;
};

struct fifo_t {
    int i_cnt;
    int last_idx;
    struct fifo_node *last, *first;
};

/**
 * Check items count in fifo and
 * return 1 if fifo is empty or 0, if fifo
 * have some elements.
 *
 * We`ll use it for check opportunity
 * for free memory.
 */
int empty(struct fifo_t **fifo) {
    return (*fifo)->i_cnt > 0 ? 0 : 1;
}

/**
 * Create new fifo.
 */
struct fifo_t **new_fifo(int last_idx) {
    struct fifo_t *n_fifo, **ptr;
    if (last_idx < 0) {
        return NULL;
    }
    n_fifo = malloc(sizeof(*n_fifo));
    n_fifo->first = NULL;
    n_fifo->last = NULL;
    n_fifo->last_idx = last_idx;
    n_fifo->i_cnt = 0;
    ptr = &n_fifo;
    return ptr;
}

/**
 * Free memory from top-level fifo struct.
 * If fifo is empty, we free mem, set ptr
 * as NULL and return 1 as success.
 *
 * In case fifo not empty, return FIFO_NEMPTY 
 * on fail.
 */
int del_fifo(struct fifo_t **fifo) {
    if (!empty(fifo)) {
        return FIFO_NEMPTY;
    }
    free(*fifo);
    fifo = NULL;
    return 1;
}

void push_back(struct fifo_t **fifo, unsigned long hash, void *record, int *idx) {
    struct fifo_node *node;
    if (fifo == NULL) {
        *idx = FIFO_NULL_PTR;
        return;
    }
    node = malloc(sizeof(*node));
    node->id = (*fifo)->last_idx;
    node->hash = hash;
    node->fdata = record;
    node->prev = NULL;
    node->next = NULL;

    if(!(*fifo)->last) {
        (*fifo)->last = node;
        (*fifo)->first = (*fifo)->last;
    } else {
        (*fifo)->last->next = node;
        node->prev = (*fifo)->last;
        node->next = NULL;
        (*fifo)->last = node;
    }
    (*fifo)->last_idx++;
    (*fifo)->i_cnt++;
    *idx = node->id;
}

void *pop_front(struct fifo_t **fifo, int *idx) {
    struct fifo_node *tmp;
    void *fdata;
    if ((fifo == NULL) || ((*fifo)->first == NULL)) {
        *idx= FIFO_NULL_PTR;
        return NULL;
    }
    tmp = (*fifo)->first;
    (*fifo)->first = tmp->next;
    if ((*fifo)->first) {
        (*fifo)->first->prev = NULL;
    } else {
        (*fifo)->last = NULL;
    }
    fdata = tmp->fdata;
    *idx = tmp->id;
    (*fifo)->i_cnt--;
    free(tmp);
    return fdata;
}

/**
 * We can find wished element in FIFO,
 * if it contains.
 *
 * Other case we return 0 (not found)
 */
void *find_by_hash(struct fifo_t **fifo, unsigned long hash, int *idx) {
    struct fifo_node *first;
    if (fifo == NULL) {
        *idx = FIFO_NULL_PTR;
        return NULL;
    }
    first = (*fifo)->first;
    while (first) {
        if (first->hash == hash) {
            *idx = first->id;
            return first->fdata;
        }
        first = first->next;
    }
    *idx = FIFO_NOTFND;
    return NULL;
}

/**
 * Pop item form fifo by hash
 * Item have to free mem after.
 * It looks ugly, but is working...
 */
void *pop_by_hash(struct fifo_t **fifo, unsigned long hash, int *idx) {
    struct fifo_node *first;
    void *fdata;
    if (fifo == NULL) {
        *idx = FIFO_NULL_PTR;
        return NULL;
    }
    first = (*fifo)->first;
    while (first) {
        if (first->hash == hash) {
            if (first->next) {
                first->next->prev = first->prev;
            }
            if (first->prev) {
                first->prev->next = first->next;
            }
            if (first == (*fifo)->first) {
                (*fifo)->first = first->next;
            }
            if (first == (*fifo)->last) {
                (*fifo)->last = first->prev;
            }
            *idx = first->id;
            fdata = first->fdata;
            free(first);
            (*fifo)->i_cnt--;
            return fdata;
        }
        first = first->next;
    }
    *idx = FIFO_NOTFND;
    return NULL;
}
