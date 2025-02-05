#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "kernel/types.h"
#include "user/user.h"

/*
 * find - searches for files with a specific name in a directory tree
 *
 * This is a simplified version of the UNIX find command that searches
 * recursively through directories to find files matching an exact name.
 *
 * Parameters:
 *   path   - The starting directory path to search from
 *   target - The filename to search for
 */
void find(char *path, char *target) {
  int fd;
  struct stat st;
  struct dirent de;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (st.type == T_DIR) {
    char buf[512];
    // this check that
    // len of path plus one of the '/' + the dirname + null terminating
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
      printf("find: path too long\n");
      close(fd);
      return;
    }

    // Construct the base of new paths:
    strcpy(buf, path);           // Copy current path
    char *p = buf + strlen(buf); // Point to end of current path
    *p++ = '/';                  // Add separator and move pointer

    // Read directory entries
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0) // skips empty dirs
        continue;

      // Skip . and .. to avoid inf loop
      if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;

      // Construct full path by adding the entry name
      memmove(p, de.name, DIRSIZ); // add filename to path
      p[DIRSIZ] = 0;               // Null terminate

      if (strcmp(de.name, target) == 0) {
        printf("%s\n", buf);
      }

      // Recursively call find with new path
      find(buf, target);
    }
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc != 3) { // Program name counts as first argument
    fprintf(2, "Usage: find <path> <filename>\n");
    exit(1);
  }

  char *path = argv[1];
  char *target = argv[2];
  find(path, target);
  exit(0);
}
