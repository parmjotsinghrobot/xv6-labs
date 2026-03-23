#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    // exit if there are no arguments
    if (argc == 1) {
        write(2,
            "findppid: missing operand\n"
            "Try 'findppid n' with n being a positive integer.\n", 77);
        exit(1);
    }
    int chain = atoi(argv[1]);
    while (chain >= 0) {
        int pid = fork();
        if (pid == 0) {
            chain--;
        } else {
            wait(0);
            printf("my ID is %d, my parent ID is %d\n", getpid(), getppid());
            exit(0);
        }
    }
    wait(0);
    exit(0);
}
