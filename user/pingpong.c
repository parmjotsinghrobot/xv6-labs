#include "kernel/types.h"
#include "user/user.h"

void child(int read_pipe, int write_pipe) {
    char msg[4];
    read(read_pipe, msg, 4);
    if (strcmp(msg, "ping") != 0) exit(1);

    printf("%d: pong\n", getpid());
    write(write_pipe, "ACK", 4);
    exit(0);
}

int main(int argc, char *argv[]) {
    int p_to_c1[2], c1_to_p[2];
    int p_to_c2[2], c2_to_p[2];

    pipe(p_to_c1); pipe(c1_to_p);
    pipe(p_to_c2); pipe(c2_to_p);

    int c1 = fork();
    if (c1 == 0) {
        // close unneeded pipes
        close(p_to_c1[1]);
        close(c1_to_p[0]);

        close(p_to_c2[0]); close(p_to_c2[1]);
        close(c2_to_p[0]); close(c2_to_p[1]);

        child(p_to_c1[0], c1_to_p[1]);
    }

    int c2 = fork();
    if (c2 == 0) {
        // close unneeded pipes
        close(p_to_c2[1]);
        close(c2_to_p[0]);

        close(p_to_c1[0]); close(p_to_c1[1]);
        close(c1_to_p[0]); close(c1_to_p[1]);

        child(p_to_c2[0], c2_to_p[1]);
    }

    // children will exit in the function and not exec this code

    // close unneeded pipes
    close(p_to_c1[0]); close(p_to_c2[0]);
    close(c1_to_p[1]); close(c2_to_p[1]);

    write(p_to_c1[1], "ping", 5);
    write(p_to_c2[1], "ping", 5);

    char ack[4];
    read(c1_to_p[0], ack, 3);
    if (strcmp(ack, "ACK")) exit(1);
    read(c2_to_p[0], ack, 3);
    if (strcmp(ack, "ACK")) exit(1);

    // wait for both children before exiting
    wait(0);
    wait(0);
    exit(0);
}
