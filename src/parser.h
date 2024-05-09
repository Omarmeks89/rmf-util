#ifndef _PARSER_H_ENTRY
#define _PARSER_H_ENTRY

/* 1 symb for '/' and next one for '\0' */
#define SET_FNAME_SYMBS_CNT(size) ((int)(size + 2))
#define PATH_PARTS_SEP '/'
#define SUFF_SEP '.'

#define ALLOWED_NAME_SIZE_LIMIT 255

#ifdef __cplusplus
extern "C"
#endif      /* __cplusplus */

int set_fname_borders(const char *fname, int *begin, int *size);
char *split_fname(const char *fname, int begin, int size);
int get_strlen(const char *str);
char *set_new_path(const char *new_fname, const char *root_path, int name_len, int path_len);
void free_fname(char *splitted_fname);

#endif      /* _PARSER_H_ENTRY */
