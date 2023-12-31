#ifndef RMF_FIFO_ENTRY
#define RMF_FIFO_ENTRY

/**
 * Specify ptr to fifo node
 */

#ifdef __cplusplus
extern "C"
#endif

/* top-level struct to operate with FIFO */
typedef struct fifo_t **rmf_fifo_t;
typedef void *dstruct_t;

typedef enum fifo_errors {
    FIFO_NULL_PTR = -1,
    FIFO_NOTFND =   -2,
    FIFO_NEMPTY =   -3,
} f_errors;

void push_back(rmf_fifo_t fifo, unsigned long hash, void *rec, int *idx);

/**
 * Pop first element and remove it
 */
dstruct_t pop_front(rmf_fifo_t fifo, int *idx);

/**
 * Find element by hash without removing.
 */
dstruct_t find_by_hash(rmf_fifo_t fifo, unsigned long hash, int *idx);
dstruct_t pop_by_hash(rmf_fifo_t fifo, unsigned long hash, int *idx);
rmf_fifo_t new_fifo(int last_idx);
int empty(rmf_fifo_t fifo);

/* fifo destructor */
int del_fifo(rmf_fifo_t fifo);

#endif
