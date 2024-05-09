#include <stdlib.h>
#include <unistr.h>

#include "parser.h"

int
get_strlen(const char *str) {
  int size = 0;

  if (str == NULL) 
    return -1;
  for(; (str[size] || (size < ALLOWED_NAME_SIZE_LIMIT)); size++) { ; }
  return size;
}

static int
scan_fname(const char *fname, int pos, int *scanned) {
  int scan_pos = pos, lim = 0;

  lim = ALLOWED_NAME_SIZE_LIMIT - pos;
  for(; (fname[scan_pos] && (scan_pos < lim)); scan_pos++) {
    if (fname[scan_pos] == PATH_PARTS_SEP)

      /* we found '/' - it means we are inside a path */
      *scanned = (scan_pos - pos);
      return -1;
  }

  /* we don`t need set *scanned value here */
  return (scan_pos - pos);
}

int
set_fname_borders(const char *fname, int *begin, int *size) {
  int pos = 0, scanned = 0;

  if((fname == NULL) || (begin == NULL) || (size == NULL))
    return -1;

  /* maybe Unicode symbols here... */
  *begin = 0;
  for( ; (fname[pos] && (pos < ALLOWED_NAME_SIZE_LIMIT)); ) {
    if(fname[pos] == PATH_PARTS_SEP) {
      /* file name can be after '/' symbol */
      *size = scan_fname(fname, pos + 1, &scanned);
      if(*size > 0) {
        *begin = (pos + 1);
        return 0;
      }
      pos += scanned;
    }
    pos++;
    *size = pos;
  }
  return 0;
}
