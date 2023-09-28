#ifndef RMF_FIFO_ENTRY
#define RMF_FIFO_ENTRY

/**
 * Specify ptr to fifo node
 */
typedef struct fifo_t *rmf_fifo_t;

int push_back(rmf_fifo_t fifo, void *rec);
/**
 * Pop first element and remove it
 */
void *pop_front(rmf_fifo_t fifo);

/**
 * Find element by name if we want
 * to restore some file.
 */
void *find_by_fname(rmf_fifo_t fifo);
rmf_fifo_t new_fifo(int id, char *fname, void *fdata);

#endif
