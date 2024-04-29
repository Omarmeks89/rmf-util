#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fnctl.h>

#define _RMF_ROOT_DIR "/var/lib/rmf"
#define NAME_SIZE_LIMIT 255

/* add 1 for '/' and 1 for '\0' */
#define _ADD_SYMBOLS 2

int addlink(const char *oldpath, const char *newpath) {
  /* is oldpath symlink? */
  int stat_res = 0, lnk_res = 0;

# if defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS == 64
  /* this case we schould use stat64 */
  struct stat64 *f_stat;
  
  f_stat = (struct stat64*)calloc(1, sizeof(struct stat64));
  if (f_stat == NULL) {
    /* memory was not allocated */
    perror("memory not allocated");
    return -1;

  }
  /* f_stat is a pointer */
  stat_res = stat64(oldpath, f_stat);
  if (stat_res < 0) {
    printf("stat for file <%s> not received. Error %s\n", oldpath, strerror(errno));
    free(f_stat);
    return -1;
  }

# else            /* _FILE_OFFSET_BITS not defined or < 64 */

  /* handle stat if _FILE_OFFSET_BITS is not defined
   * or it`s lower as 64 */
  struct stat *f_stat;

  f_stat = (struct stat*)calloc(1, sizeof(struct stat));
  if (f_stat == NULL) {
    perror("memory not allocated");
    return -1;
  }

  stat_res = stat(oldpath, f_stat);
  if (stat_res < 0) {
    printf("stat for file <%s> not received. Error %s\n", oldpath, strerror(errno));
    free(f_stat);
    return -1;
  }

# endif           /* _FILE_OFFSET_BITS end */

  /* check on symlink */
  if (S_ISLNK(*(f_stat->st_mode)) {
    /* this is symlink, try to follow it */ 
    lnk_res = linkat(oldpath, newpath, AT_SYMLINK_FOLLOW);
  } else {
    lnk_res = linkat(oldpath, newpath);
  }

  /* may we check on DIR, FIFO, etc? */
  if (lnk_res < 0) {
    printf("file <%s> not saved\n by new path: <%s>.\nError %s\n", oldpath, newpath, strerror(errno));
    free(f_stat);
    return -1;

  return 0;
}
