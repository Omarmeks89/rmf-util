#include <stdio.h>
#include <stdlib.h>

/**
 * Mask for fetch current file permissions / mode.
 * Mask is 12 bits only: 0000 1111 1111 1111.
 */
#define DF_PERM_MASK 0x0FFF

/**
 * Define mask for fetching file num from <credentials>.
 */
#define FNUM_F_MASK 0xFFFFFFFF

/**
 * Opcodes.
 * Range from 1 to 31 (0x01 -> 0x1F).
 */
#define OP_MASK        (char)0x1F
#define OP_NULL        (char)0x00
#define OP_FINFO       (char)0x01
#define OP_FDEL        (char)0x02
#define OP_FREST       (char)0x03
#define OP_FSTOR       (char)0x04

/**
 * applying flag:
 *     0x20 => FOR_ALL;
 *     0x00 => SINGLE_APPL;
 */
#define APPFL_ALL      (char)0x20
#define APPFL_SING     (char)0x00

/**
 * Define opcode parsing errors.
 * <opcode_t> is a signed char and
 * we using values < 0 as err_codes.
 */
#define OP_ERR_NULL    (char)0x80
#define OP_REPT_ERR    (char)0x81
/* Uncnown opcode symbol. */
#define OP_UNKN_ERR    (char)0x82
/**
 * Too much args - if we found sth after
 * op_flags and APPFL_ALL installed.
 * (in this case we don`t agree any args
 * because we apply command to all items)
 */
#define OP_TMUCH_ERR   (char)0x83

/* type, represents a field in .mdf file */
typedef unsigned long r_field_t;
typedef char opcode_t;

/**
 * Ranges for shift masks and
 * install values to wished bits.
 */
enum shifts_range {
    max_h_shift = 31,
    fnum_shift = 32,
} sh_range;

/**
 * Set limits for some income params,
 */
enum app_limits {
    fname_lim = 255,
} a_limits;

/* Set fnum into record <credentials>. */
int set_fnum(r_field_t *field, int num);
int get_fnum(r_field_t *field);
r_field_t nhash(const char *fname, int maxlen);
int get_fperms(void *fperms);
void parse_flag(
        const char *arg,
        opcode_t *code,
        opcode_t *flg,
        opcode_t *err
        );

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
 *     (size of stored file (chars))
 * 
 * Current type size: 8 * 4 = 32 chars.
 */
struct f_record {
    r_field_t credentials;
    r_field_t nhash;
    r_field_t *path_ptr;
    r_field_t i_node;
    r_field_t size;
};

/**
 * Header for .mdf file.
 * Size (as a record) = 32 bytes:
 *      [0 - 23] -> data;
 *      [23 - 31] -> alignment.
 */
struct mdf_header {
    short max_files;                /* max count of files, that app can store. */
    int max_fsize_bytes;            /* limit for a single file size (bytes) */
    short store_lim;                /* setted limit for stored files cnt (<= max_files) */
    unsigned int c_size_bytes;
    /* will used since v0.1.1 */
    unsigned long max_vol_bytes;    /* max size (bytes) that app is allowed to store */
};

/* Produce 64-bit hash value for filename */
r_field_t
nhash(const char *fname, int maxlen) {
    r_field_t hash = 1, head, tail, m, m_shift;
    if (fname == NULL)
        return hash & 0;
    for(int i = 0; fname[i]; i++) {
        if (!(i ^ maxlen)) {
            return hash & 0;
        }
        m = (r_field_t)fname[i];
        m_shift = (m ^ 0x07) & (i ^ 0x07);
        head = ((m << max_h_shift) | ((i ^ m) << 0x0A));
        tail = m << m_shift;
        hash += head | tail;
    }
    hash = hash << (m_shift ^ 0x07);
    return (r_field_t)hash;
}

/**
 * We need to have side-effect here, bcs
 * we can get NULL pointer as a parameter
 * and we have to inform about.
 */
int
set_fnum(r_field_t *field, int num) {
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

int
get_fnum(r_field_t *field) {
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
int
get_fperms(void *fperms) {
    /* Check on NULL pointer. */
    r_field_t *fld;
    if (fperms == NULL)
        return -1;
    fld = (r_field_t*)fperms;
    return (int)(*fld & DF_PERM_MASK);
}

/**
 * Parse args from CLI.
 * As result we set opcode or set
 * na error (if occured).
 */
opcode_t
parse_command(const char *argv[], opcode_t *err) {
    opcode_t appl_flg = APPFL_SING;
    opcode_t code = OP_NULL;

    for(; *argv; argv++) {
        if (*err ^ OP_ERR_NULL) {
            break;
        }

        switch (*argv[0]) {
            case '-':
                parse_flag(++*argv, &code, &appl_flg, err);
                break;
            default:
                /**
                 * If no command detected, it means that
                 * we call app without flags and this is
                 * store command.
                 *
                 * If -a flag (apply to all) specified,
                 * we set an error, bcs this case we
                 * wont`t get any args.
                 */
                if (!(code ^ OP_NULL)) {
                    code = OP_FSTOR;
                } 
                /* goto is mush more simple here */
                goto done;
        }
    }
done:
    return appl_flg | code;
}

/**
 * Parse flag into opcode,
 * or set error, if raised.
 * Avalaible flags syntax:
 *      -d | -a | -i | -r
 *      -da | -ia | -ra 
 *      -ad | -ai | -ar
 *      -d -a | -i -a | -a -r
 *      (and reversed order for last line)
 */
void
parse_flag(const char *arg, opcode_t *code, opcode_t *flg, opcode_t *err) {

    while ((*arg) && (*err == OP_ERR_NULL)) {

        /* if we have not null opcode and it`s not 'a' flag - dup args, error */
        if ((*code ^ OP_NULL) && (*arg ^ 'a')) {
            *err = OP_TMUCH_ERR;
            break;
        }

        switch (*arg) {
            case 'a':
                /**
                 * If we found one more <a> specifyer
                 * we`ll raise error (repeated arg).
                 */
                if (!(*flg ^ APPFL_ALL)) {
                    *err = OP_REPT_ERR;
                } else {
                    *flg = APPFL_ALL;
                }
                break;
            case 'i':
                *code = OP_FINFO;
                break;
            case 'r':
                *code = OP_FREST;
                break;
            case 'd':
                *code = OP_FDEL;
                break;
            default:
                *err = OP_UNKN_ERR;
                break;
        }
        arg++;
    }
    /* if error detected - do nothing (OP_NULL) */
    if (*err ^ OP_ERR_NULL) {
        *code = OP_NULL;
    }
}

/* main loop & opcodes parsing */
int main(int argc, const char *argv[]) {
    opcode_t err_t = OP_ERR_NULL, code_t = OP_NULL, op;
    if (argc == 1) {
        printf("Test msg. App version: 0.1.0\n");
        exit(0);
    }
    code_t = parse_command(++argv, &err_t);
    if (err_t ^ OP_ERR_NULL) {
        /* call func that print human-readable error info. */
        printf("ERROR #%d | 0x%02x\n", err_t, err_t);
        exit(1);
    }
    printf("#%d | 0x%02x\n", code_t, code_t);
    /* open .mdf */
    op = code_t & OP_MASK;
    switch (op) {
        case OP_FINFO:
            /* choose what we gonna do with opcode */
            break;
        case OP_FSTOR:
            break;
        case OP_FREST:
            break;
        case OP_FDEL:
            break;
        default:
            /* unknown opcode */
            err_t = OP_UNKN_ERR;
    }
    /* write data and close .mdf */
    return 0;
}
