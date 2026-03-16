#include "kernel/types.h"
#include "user/user.h"

// parent write into pipe for 2 children
void p_pipe_write(int p1, int p2, char *c, int size_c) {
    write(p1, c, size_c);
    write(p2, c, size_c);
}

// hold the process until both children write wait_msg into their pipes
void wait_pipe_read(int p1, int p2, char *c, int size_c, char *wait_msg) {
    read(p1, c, sizeof(c));
    if (strcmp(c, wait_msg)) exit(1);
    read(p2, c, sizeof(c));
    if (strcmp(c, wait_msg)) exit(1);
}

int main(int argc, char *argv[]) {
    int ppid = getpid();

    int c1_pipe[2], c2_pipe[2];
    pipe(c1_pipe);
    pipe(c2_pipe);
    char ping[10] = "ping";
    char ack[10] = "ACK";
    char read_msg[10];

    int c1_pid = fork();
    int c2_pid;

    int *pipe[2] = {0, 0};
    
    // only fork in the parent process
    if (ppid == getpid()) {
        c2_pid = fork();
        // assign pipes based on child
        if (ppid != getpid()) {
            pipe[0] = &c2_pipe[0];
            pipe[1] = &c2_pipe[1];
        }
    } else {
        // assign pipes based on child
        pipe[0] = &c1_pipe[0];
        pipe[1] = &c1_pipe[1];
    }

    if (getpid() == ppid) {
        // send ping to the children
        p_pipe_write(
            c1_pipe[1],
            c2_pipe[1],
            ping,
            sizeof(ping));
        sleep(1);
        // wait for both ack responses
        wait_pipe_read(
            c1_pipe[0],
            c2_pipe[0],
            read_msg,
            sizeof(read_msg),
            ack);
        wait(&c1_pid);
        wait(&c2_pid);
    } else {
        // wait for ping
        read(*pipe[0], read_msg, sizeof(read_msg));
        if (strcmp(read_msg, ping)) exit(1);        // exit if wrong response
        printf("%d: pong\n", getpid());
        write(*pipe[1], ack, sizeof(ack));
        if (getpid() != c1_pid) {
            // let the first child print first (generally)
            wait(&c1_pid);
        }
        // write(2, "child\n", 8);
    }
    exit(0);
}
