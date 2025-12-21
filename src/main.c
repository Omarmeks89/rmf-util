#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <locale.h>
#include <errno.h>
#include <string.h>

#include "errors.h"

#define RMF_ROOT_PATH_TAIL "/.rmf/"
#define RMF_ROOT_TAIL_LEN 6

/*
 * ADD_TAIL add additional symbol for '\0'
 */
#define ADD_TAIL(sz) (sz + 1)

#define OS_PATH_SEP '/'

#define LNKSIZE 512

#define MAX_FLAG_LEN 2

static void addlink(const char *oldpath, const char *newpath)
{
    handle_err(link(oldpath, newpath), 0, strerror(errno));
}

static void removelink(const char *fpath)
{
    handle_err(unlink(fpath), 0, strerror(errno));
}

/** \fn sfname lookup for start filename position inside a full path.
 * If path ended as '/' will be returned NULL.
 *
 * @param path              full path;
 * @param pathlen           full path len;
 */
static const char *
sfname(const char *path, size_t pathlen)
{
    size_t i = pathlen - 1;
    if (path[i] == OS_PATH_SEP)
        return NULL;

    for (; i > 0; i--)
    {
        if (path[i] == OS_PATH_SEP)
        {
            i++;
            break;
        }
    }

    return (const char *)(path + i);
}

static void
mkpath(char *buf, const char *fname, size_t fname_len)
{
    const char *home_dir = getenv("HOME");
    handle_null(home_dir, "var 'HOME' not found");

    size_t path_len = strlen(home_dir);
    if ((path_len + RMF_ROOT_TAIL_LEN + ADD_TAIL(fname_len)) > LNKSIZE)
        raise_err("too large path");

    snprintf(buf, LNKSIZE, "%s%s%s", home_dir, RMF_ROOT_PATH_TAIL, fname);
}

void rmlink(const char *fpath)
{
    char buf[LNKSIZE] = {};
    size_t pathlen = strlen(fpath);
    if (pathlen == 0)
        raise_err("empty path");

    const char *fname = sfname(fpath, pathlen);
    handle_null(fname, "invalid filename");

    mkpath(buf, fname, strlen(fname));
    removelink(buf);
}

void newlink(const char *fpath)
{
    char buf[LNKSIZE] = {};
    size_t pathlen = strlen(fpath);
    if (pathlen == 0)
        raise_err("empty path");

    const char *fname = sfname(fpath, pathlen);
    handle_null(fname, "invalid filename");

    mkpath(buf, fname, strlen(fname));
    addlink(fpath, buf);
}

void process_command(const char *flag, const char *fpath)
{
    size_t flag_size = strlen(flag);
    if ((flag_size != MAX_FLAG_LEN) || (flag[0] != '-'))
        raise_err("invalid flag");

    flag++;

    /* flags: -a (add file), -r (remove file) */
    switch (*flag)
    {
    case 'r':
        rmlink(fpath);
        break;
    case 'a':
        newlink(fpath);
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

    process_command(flag, fpath);

    return 0;
}
