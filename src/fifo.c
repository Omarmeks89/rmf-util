#include <stdlib.h>

#include "fifo.h"

struct fifo_t {
    int id;
    char *fname;
    void *fdata;
    struct fifo_t *prev, *next;
};

/**
 * Create new fifo.
 */
struct fifo_t *new_fifo(int id, char *fname, void *fdata) {
    struct fifo_t *n_fifo;
    if ((fname == NULL) || (fdata == NULL) || (id < 0)) {
        return NULL;
    }
    n_fifo = malloc(sizeof(struct fifo_t));
    n_fifo->id = id;
    n_fifo->fname = fname;
    n_fifo->fdata = fdata;
    return n_fifo;
}
