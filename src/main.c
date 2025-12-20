#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <locale.h>
#include <errno.h>
#include <string.h>

#include "errors.h"

#define RMF_ROOT_PATH_TAIL "/.rmf"

#define OS_PATH_SEP '/'

#define DEF_LINKPATH_LEN 512

#define MAX_FLAG_LEN 2

void addlink(const char *oldpath, const char *newpath)
{
    handle_err(link(oldpath, newpath), 0, strerror(errno));
}

void removelink(const char *fpath)
{
    handle_err(unlink(fpath), 0, strerror(errno));
}

/** \fn set_root_path build path to application
 * root directory (set pointer)
 * @param strbuf             memory allocated for root path string */
void set_root_path(char *strbuf)
{
    const char *home_dir = getenv("HOME");
    handle_null(home_dir, "var 'HOME' not found");

    size_t path_len = strlen(home_dir);
    size_t rmf_tail_len = strlen(RMF_ROOT_PATH_TAIL);

    /* 1 - symbol '\0' */
    if ((path_len + rmf_tail_len + 1) > DEF_LINKPATH_LEN)
        raise_err("too large path");

    // replaced sprintf on snprintf to check string limits.
    snprintf(strbuf, DEF_LINKPATH_LEN, "%s%s", home_dir, RMF_ROOT_PATH_TAIL);
}

/** \fn set_link_path build path for new link
 * @param buf               memory allocated to store link path
 * @param root_path         path to application root directory (as link path part)
 * @param fname             pointer on filename (start symbol) */
void set_link_path(char *buf, char *root_path, char *fname)
{
    size_t root_len = strlen(root_path);
    size_t fname_len = strlen(fname);

    /* 2 - symbol '/' and '\0' symbol */
    if ((root_len + fname_len + 2) > DEF_LINKPATH_LEN)
        raise_err("too large path");

    snprintf(buf, DEF_LINKPATH_LEN, "%s/%s", root_path, fname);
}

/** \fn lookup_fname return filename start index
 * @param fpath             full qualified path to file
 * @return                  filename start index */
size_t lookup_fname(const char *fpath_ptr)
{
    const char *start_path_ptr = fpath_ptr;
    size_t start_name_idx = 0;

    for (; *fpath_ptr; fpath_ptr++)
    {

        if (*fpath_ptr == OS_PATH_SEP)
        {
            start_name_idx &= 0;
            continue;
        }

        if (start_name_idx == 0)
        {
            start_name_idx = (size_t)(fpath_ptr - start_path_ptr);
        }
    }

    return start_name_idx;
}

/** \fn get_fname return pointer on filename or
 * return NULL, if nullpointer provided as an argument
 * @param fpath             fullpath to file
 * @param buf               memory allocated for cleaned path
 * @return                  pointer on filename start symbol (or NULL) */
char *get_fname(const char *fpath, char *buf)
{
    size_t st_name = 0;
    void *tptr = NULL;

    tptr = realpath(fpath, buf);
    handle_null(tptr, "path not resolved");
    st_name = lookup_fname(buf);

    return buf + st_name;
}

/** \fn create_link create new hard link on file
 * @param fpath             path to file (or filename) you want to store */
void create_link(const char *fpath)
{
    char *rootpath_buf = NULL, *fullpath = NULL, *fname = NULL, *fname_startptr = NULL;
    handle_null(fpath, "no path");

    fname = (char *)malloc(DEF_LINKPATH_LEN * sizeof(char));
    handle_null(fname, "allocation failed");

    fname_startptr = get_fname(fpath, fname);
    handle_null(fname_startptr, "error on lookup filename");

    /* TODO: make one allocation */
    rootpath_buf = (char *)malloc(DEF_LINKPATH_LEN * sizeof(char));
    handle_null(rootpath_buf, "allocation failed");
    set_root_path(rootpath_buf);

    fullpath = (char *)malloc(DEF_LINKPATH_LEN * sizeof(char));
    handle_null(fullpath, "allocation failed");
    set_link_path(fullpath, rootpath_buf, fname_startptr);

    addlink(fpath, fullpath);

    free(fullpath);
    free(rootpath_buf);
    free(fname);
}

/** \fn remove_link make unlink for wished file.
 * If file not exists in rmf catalogs error will be raised.
 * @param fpath             path to file you want to remove
 */
void remove_link(const char *fpath)
{
    char *rootpath_buf = NULL, *fullpath = NULL, *fname = NULL, *fname_startptr = NULL;
    handle_null(fpath, "no path");

    fname = (char *)malloc(DEF_LINKPATH_LEN * sizeof(char));
    handle_null(fname, "allocation failed");

    fname_startptr = get_fname(fpath, fname);
    handle_null(fname_startptr, "error on lookup filename");

    rootpath_buf = (char *)malloc(DEF_LINKPATH_LEN * sizeof(char));
    handle_null(rootpath_buf, "allocation failed");
    set_root_path(rootpath_buf);

    fullpath = (char *)malloc(DEF_LINKPATH_LEN * sizeof(char));
    handle_null(fullpath, "allocation failed");
    set_link_path(fullpath, rootpath_buf, fname_startptr);

    removelink(fullpath);

    free(fullpath);
    free(rootpath_buf);
    free(fname);
}

/* flags: -a (add file), -r (remove file) */

void exec_command(const char *flag, const char *fpath)
{
    size_t flag_size = strlen(flag);
    if (flag_size != MAX_FLAG_LEN)
        raise_err("invalid flag");

    flag++;
    switch (*flag)
    {
    case 'r':
        remove_link(fpath);
        break;
    case 'a':
        create_link(fpath);
        break;

    default:
        raise_err("unknown flag");
    }
}

int main(int argc, char *argv[])
{
    char *flag = NULL, *fpath = NULL;
    handle_err(argc, 3, "not enough args");
    handle_null(setlocale(LC_ALL, ""), "locale not set");

    flag = argv[1];
    fpath = argv[2];

    handle_null(flag, "empty flag");
    handle_null(fpath, "no file path");

    exec_command(flag, fpath);

    return 0;
}
