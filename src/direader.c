#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include "direader.h"
#include "fifo.h"

static int mangle_fname(const char *fname,
                        char *mname,
                        int size);

/* Mangle exact filename using '.' as a first
 * symbol in name. */
static int
mangle_fname(const char *fname, char *mname, int size) {
    if ((!fname) || (!mname) || (size <= 0)) 
        return -1;
    *mname = '.';
    mname++;
    for (; *fname; ) {
        *mname = *fname;
        mname++;
        fname++;
    }
    *(mname + 1) = '\0';
    return 1;
}
