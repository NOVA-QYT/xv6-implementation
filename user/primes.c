#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Defines a process in the prime sieve pipeline.
void process(int pfd[]) {
    int prime;

    // Attempt to read a prime number from the input pipe.
    if (read(pfd[0], &prime, sizeof(int)) == 0) {
        // Exit if there's no data (indicating the pipe is closed).
        close(pfd[0]);
        exit(0);
    }
    
    // Print the prime number that has been identified.
    printf("prime %d\n", prime);

    // Set up a new pipe for further prime number filtering.
    int p2[2];
    pipe(p2);

    // Create a new process.
    int pid = fork();
    if (pid == 0) {
        // Child process: close the unused write end and recursively process further numbers.
        close(p2[1]);
        process(p2);
    } else if(pid > 0) {
        // Parent process: read subsequent numbers and forward them if they are not divisible by the current prime.
        int n;
        while (read(pfd[0], &n, sizeof(n)) > 0) {
            if (n % prime != 0) {
                write(p2[1], &n, sizeof(n));
            }
        }
        // Close the read end of the input pipe and the write end of the output pipe after processing all numbers.
        close(pfd[0]);
        close(p2[1]);
        // Wait for the child process to terminate before exiting.
        wait(0);
        exit(0);
    }   
}

int main(void) {
    // Initialize the first pipe.
    int pfd[2];
    pipe(pfd);

    // Start the prime sieve pipeline by forking the first process.
    int pid = fork();
    if (pid == 0) { 
        // Child process: Close the write-end of the pipe and start the filtering process.
        close(pfd[1]);
        process(pfd);
    } else {
        // Parent process: Writes the sequence of numbers to be filtered into the pipe.
        close(pfd[0]); // Close the unused read end.
        for (int i = 2; i <= 35; i++) {
            write(pfd[1], &i, sizeof(int));
        }
        close(pfd[1]); // Close the write end to signal the end of input.
        wait(0); // Wait for the entire filtering process to complete.
    }
    exit(0);
}
