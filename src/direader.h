#ifndef DIREADER_ENTRY
#define DIREADER_ENTRY

#ifdef __cplusplus
extern "C"
#endif

struct _FINFO {
    int f_ino;          /* index descriptor no. */
    /* use x & 0x0FFF here to split file permissions */
    int f_mode;         /* file operation mode */
    int f_size;         /* file size bytes from <struct stat> */
};

/* we share pointer to this struct, not a struct */
typedef struct _FINFO *FINFO;

/* If we start at first time we will 
 * create .tmpdir to store files and empty
 * .mdf file. Other case we`ll do nothing.
 * <.tmpdir> will be created inside your home-dir. */
void init_tmpdir(const char *tmp_dirname);

/* temporary store removed file from
 * <oldpath> and create new HL in <newpath>.
 * if <perms> > 0, it will be used for
 * <chmod> call inside a func.
 * Return:
 *      int (curr file fd) */
int tmpstore_file(const char *oldpath,
                   const char *newpath,
                   int perms);

/* remove file from temporary storage.
 * Return:
 *      1 -> on success;
 *      0 -> (or >= 0) on fail */
int remove_tmpstored(const char *name,
                     int flags);

/* get info about temporary stored file.
 * Incapsulate stat / fstat under the hood.
 * Return:
 *      1 -> on success;
 *      0 -> (or >= 0) on fail */
int get_finfo(const char *fname,
                 FINFO finfo);

/* Interface for FINFO pointer.
 * Use it to fetch value from ptr.
 * Shouldn`t use ptr directry, struct
 * may change version by version, so
 * think about back capability */
int get_fsize(FINFO finfo);
int get_fmode(FINFO finfo);
int get_finode(FINFO finfo);
#endif
