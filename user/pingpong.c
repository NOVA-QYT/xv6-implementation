#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
    int p2c[2], c2p[2]; // Arrays to hold pipe file descriptors (parent to child, child to parent)
    char buf[1]; // Buffer to hold the byte being sent
    int pid;

    // Create the two pipes
    pipe(p2c);
    pipe(c2p);

    // Fork a child process
    pid = fork();

    if (pid < 0) {
        // Fork failed
        fprintf(2, "fork failed\n");
        exit(1);
    } else if (pid == 0) {
        // Child process
        close(p2c[1]); // Close unused write end of parent-to-child pipe
        close(c2p[0]); // Close unused read end of child-to-parent pipe

        read(p2c[0], buf, 1); // Read a byte from the parent
        printf("%d: received ping\n", getpid());

        write(c2p[1], buf, 1); // Send a byte back to the parent
        close(p2c[0]);
        close(c2p[1]);
        exit(0);
    } else {
        // Parent process
        close(p2c[0]); // Close unused read end of parent-to-child pipe
        close(c2p[1]); // Close unused write end of child-to-parent pipe

        buf[0] = 'x'; // The byte to send
        write(p2c[1], buf, 1); // Send a byte to the child

        wait(0); // Wait for child to finish

        read(c2p[0], buf, 1); // Read the byte sent back by the child
        printf("%d: received pong\n", getpid());

        close(p2c[1]);
        close(c2p[0]);
        exit(0);
    }
}
