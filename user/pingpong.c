#include "kernel/types.h"
#include "user/user.h"

int main() {
  int pipe1[2];
  int pipe2[2];
  char byte = 'x'; // The actual byte value doesn't matter

  if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
    fprintf(2, "pipe creation failed\n");
    exit(1);
  }

  int pid = fork();
  if (pid < 0) {
    fprintf(2, "fork failed\n");
    exit(1);
  }

  if (pid == 0) {    // child process
    close(pipe1[1]); // Close write end of pipe1
    close(pipe2[0]); // Close read end of pipe2

    // Receive byte from parent
    if (read(pipe1[0], &byte, 1) != 1) {
      fprintf(2, "child read failed\n");
      exit(1);
    }
    printf("%d: received ping\n", getpid());

    // Send byte back to parent
    if (write(pipe2[1], &byte, 1) != 1) {
      fprintf(2, "child write failed\n");
      exit(1);
    }

    close(pipe1[0]);
    close(pipe2[1]);
  } else {           // parent process
    close(pipe1[0]); // Close read end of pipe1
    close(pipe2[1]); // Close write end of pipe2

    // Send byte to child
    if (write(pipe1[1], &byte, 1) != 1) {
      fprintf(2, "parent write failed\n");
      exit(1);
    }

    // Receive byte from child
    if (read(pipe2[0], &byte, 1) != 1) {
      fprintf(2, "parent read failed\n");
      exit(1);
    }
    printf("%d: received pong\n", getpid());

    close(pipe1[1]);
    close(pipe2[0]);
  }
  exit(0);
}
