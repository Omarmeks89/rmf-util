#include <stdio.h>
#include <stdlib.h>

/**
 * Env key, for define path to storing
 * files, that we will delete.
 * We`ll make a new dir by: ${HOME}/.<fname>.
 */
# define HOME_DIR_ENV_KEY "HOME"

/**
 * Define a default value for temp-directory name.
 */
# define TEMP_FILE_DIR_NAME "/.tmpdir"

/**
 * Mask for fetch current file permissions / mode.
 * Mask is 12 bits only: 0000 1111 1111 1111.
 */
# define DF_PERM_MASK 0x0FFF

/**
 * Define mask for fetching file num from <credentials>.
 */
# define FNUM_F_MASK 0xFFFFFFFF

/* type, represents a field in .mdf file */
typedef unsigned long r_field_t;

/**
 * Ranges for shift masks and
 * install values to wished bits.
 */
static enum shifts_range {
    fnum_shift = 32,
} sh_range;

/**
 * Set limits for hash size.
 */
static enum hash_lim {
    fname_hsize = 211,
} hlim;

/**
 * Set limits for some income params,
 */
static enum app_limits {
    fname_lim = 255,
} a_limits;

/* Set fnum into record <credentials>. */
static int set_fnum(r_field_t *field, int num);
static int get_fnum(r_field_t *field);

/* Return hash from current filename. */
static r_field_t nhash(const char *fname, int maxlen);

/**
 * Fetch current file permissions / mode to
 * store it into a meta-record (.mdf file)
 */
static int get_fperms(void *fperms);

/**
 * Type, that represents any stored file
 * as a record in .mdf file.
 * We create .mdf file for collect and
 * check stored records.
 *
 * Fields:
 *     credentials: [32 bits (num)][20 bits (resrv)][12 bits (perms)];
 *     (perms means stored file origin mode)
 *     nhash: [64 bits] 
 *     (hash from stored file name)
 *     i_node: [64 bits]
 *     (num of inode, we use 64 bits here)
 *     size: [64 bits]
 *     (size of stored file (bytes))
 * 
 * Current type size: 8 * 4 = 32 bytes.
 */
static union mdata {
    struct f_record {
        r_field_t credentials;
        r_field_t nhash;
        r_field_t i_node;
        r_field_t size;
    } r_field;
    char record[sizeof(struct f_record)];
} fdata;

/**
 * Hash-func for testing purposes.
 */
r_field_t nhash(const char *fname, int maxlen) {
    /* TODO: decide about return values / side-effect codes. */
    int i, m;
    r_field_t hash = 1;
    if (fname == NULL)
        return -hash;
    for(i = 0; fname[i]; i++) {
        if (i > maxlen) {
            /**
             * we destroy hash-value, because
             * we know, that a string is bigger than
             * limit we set. So we would have collision
             * if we return calculated hash-value.
             */
            hash = -1;
            return hash;
        }
        m = (int)fname[i];
        /* we don`t care about operations order here now. */
        hash += (((i * m) + (m * m)) * (m + (i * i) * (m * m)));
        hash *= (i + m);
    }
    return (r_field_t)hash / fname_hsize;
}

/**
 * We need to have side-effect here, bcs
 * we can get NULL pointer as a parameter
 * and we have to inform about.
 */
int set_fnum(r_field_t *field, int num) {
    r_field_t fld;
    if (field == NULL)
        return -1;
    /* we needn`t set value if num is zero. */
    if (num > 0) {
        fld = (r_field_t)num << fnum_shift;
        *field = *field | fld;
    }
    return 1;
}

int get_fnum(r_field_t *field) {
    r_field_t fld;
    /* Check on null pointer. */
    if (field == NULL) 
        return -1;
    fld = (r_field_t)FNUM_F_MASK << fnum_shift;
    return (int)((*field & fld) >> fnum_shift);
}

/**
 * We don`t sure about size of file permissions in inode.
 * So we use *void to cast that type as unsigned long.
 */
int get_fperms(void *fperms) {
    /* Check on NULL pointer. */
    r_field_t *fld;
    if (fperms == NULL)
        return -1;
    fld = (r_field_t*)fperms;
    return (int)(*fld & DF_PERM_MASK);
}
