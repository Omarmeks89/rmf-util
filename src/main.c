#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define _RMF_ROOT_DIR "/var/lib/rmf"
#define NAME_SIZE_LIMIT 255

/* add 1 for '/' and 1 for '\0' */
#define _ADD_SYMBOLS 2

int addlink(const char *oldpath, const char *newpath) {
  /* is oldpath symlink? */
  int lnk_res = 0;

  lnk_res = link(oldpath, newpath);
  if (lnk_res < 0) {
    printf("file <%s> not saved.\nError %s\n", oldpath, strerror(errno));
    return -1;
  }

  return 0;
}

int main(int argc, char *argv[]) {
    int lnk_res = 0;

    if (argc < 3) {
      printf("not enough args\n");
      exit(1);
    }
    lnk_res = addlink(argv[1], "/var/lib/rmi/test.txt");
    if (lnk_res < 0) {
      exit(1);
    }
    return 0;
}
