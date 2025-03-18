#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <locale.h>
#include <pwd.h>
#include <errno.h>
#include <string.h>
#include <uniconv.h>
#include <unitypes.h>
#include <ctype.h>

#include "errors.h"

#define RMF_ROOT_PATH_TAIL                              "/.rmf"

#define OS_PATH_SEP                                     '/'

#define DEF_LINKPATH_LEN                                 512

void addlink(const char *oldpath, const char *newpath) {
    handle_err(link(oldpath, newpath), 0, strerror(errno));
}

void removelink(const char *fpath) {
    handle_err(unlink(fpath), 0, strerror(errno));
}

/** \fn set_root_path build path to application
 * root directory (set pointer)
 * @param strbuf             memory allocated for root path string */
void set_root_path(char *strbuf) {
    const char *home_dir = getenv("HOME");
    handle_null(home_dir, "var 'HOME' not found");

    size_t path_len = strlen(home_dir);
    size_t rmf_tail_len = strlen(RMF_ROOT_PATH_TAIL);

    /* 1 - symbol '\0' */
    if ((path_len + rmf_tail_len + 1) > DEF_LINKPATH_LEN)
        raise_err("too large root path");

    sprintf(strbuf, "%s%s", home_dir, RMF_ROOT_PATH_TAIL);
}

/** \fn set_link_path build path for new link
 * @param buf               memory allocated to store link path
 * @param root_path         path to application root directory (as link path part)
 * @param fname             pointer on filename (start symbol) */
void set_link_path(char *buf, char *root_path, char *fname) {
    size_t root_len = strlen(root_path);
    size_t fname_len = strlen(fname);

    /* 2 - symbol '/' and '\0' symbol */
    if ((root_len + fname_len + 2) > DEF_LINKPATH_LEN)
        raise_err("too large link path");

    sprintf(buf, "%s/%s", root_path, fname);
}

/** \fn lookup_fname return filename start index
 * @param fpath             full qualified path to file
 * @return                  filename start index */
unsigned long lookup_fname(const char *fpath) {
    const char *st_path = fpath;
    unsigned long st_name = 0;

    for (; *fpath; fpath++) {

        if (*fpath == OS_PATH_SEP) { 
            st_name = (unsigned long) 0;
            continue;
        }

        if (st_name == 0) {
            st_name = (unsigned long) (fpath - st_path);
        }
    }

    return st_name;
}

/** \fn get_fname return pointer on filename or
 * return NULL, if nullpointer provided as an argument
 * @param fpath             fullpath to file
 * @param buf               memory allocated for cleaned path
 * @return                  pointer on filename start symbol (or NULL) */
char *get_fname(const char *fpath, char *buf) {
    unsigned long st_name = 0;
    if ((fpath == NULL) || (buf == NULL))
        return NULL;

    realpath(fpath, buf);
    st_name = lookup_fname(buf);

    return buf + st_name;
}

/** \fn create_link create new hard link on file
 * @param fpath             path to file (or filename) you want to store */
void create_link(char *fpath) {
    char *strbuf = NULL, *link_strbuf = NULL, *fname = NULL;
    handle_null(fpath, "no path to file");

    fname = (char *) malloc(DEF_LINKPATH_LEN * sizeof(char));
    handle_null(fname, "allocation failed");

    fname = get_fname(fpath, fname);
    handle_null(fname, "error on lookup filename");

    /* TODO: make one allocation */
    strbuf = (char *) malloc(DEF_LINKPATH_LEN * sizeof(char));
    handle_null(strbuf, "allocation failed");
    set_root_path(strbuf);

    link_strbuf = (char *) malloc(DEF_LINKPATH_LEN * sizeof(char));
    handle_null(strbuf, "allocation failed");
    set_link_path(link_strbuf, strbuf, fpath);

    addlink(fpath, link_strbuf);

    free(link_strbuf);
    free(strbuf);
    free(fname);
}

int main(int argc, char *argv[]) {
    handle_err(argc, 2, "not enough args");
    handle_null(setlocale(LC_ALL, ""), "locale not set");

    create_link(argv[1]);
    return 0;
}
