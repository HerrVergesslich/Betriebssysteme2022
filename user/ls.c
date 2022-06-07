#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }

      char* otype = st.type == T_DIR ? "d" : "-";
      char* oread = (st.mode & (0b100 << 6)) ? "r" : "-";
      char* owrite= (st.mode & (0b010 << 6)) ? "w" : "-";
      char* oexec = (st.mode & (0b001 << 6)) ? "x" : "-";
      char* gread = (st.mode & (0b100 << 3)) ? "r" : "-";
      char* gwrite= (st.mode & (0b010 << 3)) ? "w" : "-";
      char* gexec = (st.mode & (0b001 << 3)) ? "x" : "-";
      char* read  = (st.mode & (0b100 << 0)) ? "r" : "-";
      char* write = (st.mode & (0b010 << 0)) ? "w" : "-";
      char* exec  = (st.mode & (0b001 << 0)) ? "x" : "-";

      printf("%s%s%s%s%s%s%s%s%s%s 1 %d %d %s %d %d %d\n", otype, oread, owrite, oexec, gread, gwrite, gexec, read, write, exec, st.uid, st.gid, fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}
