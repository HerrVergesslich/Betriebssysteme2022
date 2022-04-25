#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

char *fmtname(char *path) {
  static char buf[DIRSIZ + 1];
  char *p;

  // Find first character after last slash.
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if (strlen(p) >= DIRSIZ) return p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  return buf;
}

void find(char *path, char* searchName) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  //printf("Looking for %s in %s\n", searchName, path);

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
  case T_FILE:
    if(strstr(fmtname(path), searchName) >= 0) {
      printf("File/%d: %s\n", st.type, path);
    }
    break;
  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
      fprintf(2, "find: path too long\n");
      break;
    }
    if(strstr(fmtname(path), searchName) >= 0) {
      printf("Dir/%d: %s\n", st.type, path);
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de)) {
      if(de.inum == 0)
      continue;

      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0 || strlen(de.name) == 0) {
        continue;
      }

      strcpy(p, de.name);
      find(buf, searchName);
    }
    break;
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  int i;

  if (argc < 2) {
    fprintf(2, "usage: find <name> [name2] ...\n");
    exit(0);
  } else if(argc >= 2) {
    for(i = 1; i < argc; i++) {
      find(".", argv[i]);
    }
  }
  exit(0);
}
