#include "kernel/types.h"
#include "user/user.h"

// Initial Number Generation:
// The first process starts by sending numbers 2 through 280 into the pipeline.

// Filtering:
// Each process in the pipeline:

//     Reads a number from its input pipe.
//     For the first number it reads, it recognizes that number as a prime.
//     It prints the prime.
//     It then filters out (does not forward) any numbers that are divisible by
//     that prime. The remaining numbers are forwarded through another pipe to
//     the next process (which might be created on demand when the first
//     unfiltered number is encountered).

void primes(int pipe_parent[2]) __attribute__((noreturn));
void primes(int pipe_parent[2]) {
  int pipe_child[2]; // Create pipe array for communication with child process
  int prime;         // Store the prime number found
  int n;             // Store numbers being checked
  close(pipe_parent[1]); // Close write end of parent pipe since we only need to
                         // read

  // Read first number from pipe - this will be a prime
  if (read(pipe_parent[0], &prime, sizeof(int)) == sizeof(int)) {
    printf("prime %d\n", prime); // Print the prime number found

    // Create new pipe for communication with next process
    if (pipe(pipe_child) < 0) {
      fprintf(2, "pipe failed\n");
      exit(1);
    }

    if (fork() == 0) {       // Child process
      close(pipe_parent[0]); // Close parent's read end in child
      primes(pipe_child);    // Recursive call with new pipe
      exit(0);
    } else {                // Parent process
      close(pipe_child[0]); // Close read end of child pipe

      // Read remaining numbers and forward those not divisible by prime
      while (read(pipe_parent[0], &n, sizeof(int)) == sizeof(int)) {
        if (n % prime != 0) { // If number isn't divisible by prime
          write(pipe_child[1], &n, sizeof(int)); // Forward to next process
        }
      }
      close(pipe_child[1]); // Close write end after done
    }
  }

  close(pipe_parent[0]); // Close remaining parent read fd
  wait(0);               // Wait for child to finish
  exit(0);               // Exit the process
}

int main(void) {
  int pipe_parent[2];
  pipe(pipe_parent);

  if (fork() == 0) {       // child process
    primes(pipe_parent);   // execute primes function
  } else {                 // parent process
    close(pipe_parent[0]); // close reading fd
    for (int i = 2; i <= 280; i++) {
      write(pipe_parent[1], &i, sizeof(i));
    }
    close(pipe_parent[1]); // after writing in write fd close it
    wait(0);               // wait child process
  }

  exit(0);
}
