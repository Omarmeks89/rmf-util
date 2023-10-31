#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "direader.h"
#include "fifo.h"

/* Env key, for define path to storing
 * files, that we will delete.
 * We`ll make a new dir by: ${HOME}/.<fname>. */
#define HOME_DIR_ENV_KEY "HOME"

/* Define a default value for temp-directory name. */
#define TEMP_FILE_DIR_NAME "/.tmpdir"

static int mangle_fname(const char *fname,
                        char *new_fname);
