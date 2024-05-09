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

#define _RMF_ROOT_DIR "/var/lib/rmf"
#define NAME_SIZE_LIMIT 255

/* add 1 for '/' and 1 for '\0' */
#define _ADD_SYMBOLS 2

int print_curruser(void) {
  uid_t euid = 0;
  struct passwd *own_pwd;
  /* is always successfull */
  euid = geteuid();
  own_pwd = getpwuid(euid);
  if (own_pwd == NULL) {
    printf("no user with euid %ul found. Error: %s\n", euid, strerror(errno));
    return -1;
  }
  printf("process owner: %s\n", own_pwd->pw_name);
  return 0;
}

int addlink(const char *oldpath, const char *newpath) {
  int lnk_res = 0;

  /* tyr to add linkat;
   * - detect symlink;
   * - detect dir (-> error)
   * - other -> error */
  lnk_res = link(oldpath, newpath);
  if (lnk_res < 0) {
    printf("file <%s> not saved.\nError %s\n", oldpath, strerror(errno));
    return -1;
  }

  return 0;
}

int main(int argc, char *argv[]) {
    int lnk_res = 0;
    char *locale = NULL;

    if (argc < 3) {
      printf("not enough args\n");
      exit(1);
    }

    locale = setlocale(LC_CTYPE, NULL);
    if(locale == NULL) {
        /* undefined locale - exit(1) */
        printf("undefined locale <%s>\n", locale);
        exit(1);
    }
    printf("LOCALE: %s\n", locale);
    printf("_T: %s\n", locale_charset());

    lnk_res = addlink(argv[1], "/var/lib/rmf/test.txt");
    if (lnk_res < 0) {
      exit(1);
    }
    return 0;
}
