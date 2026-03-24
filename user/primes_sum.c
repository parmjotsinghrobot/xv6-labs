#include "kernel/types.h"
#include "user/user.h"

#pragma GCC diagnostic ignored "-Winfinite-recursion"
void child(int read_pipe, int prev_prime) {
    int num = 0;
    if (read(read_pipe, &num, sizeof(int)) != sizeof(int)) exit(0);
    int p = num;
    if (p != prev_prime) printf("%d\n", p + prev_prime);

    int c_to_c[2];
    pipe(c_to_c);
    int pid = fork();

    if (pid == 0) { // if child
        close(read_pipe);
        close(c_to_c[1]);
        child(c_to_c[0], p);
        exit(0);
    } else {
        close(c_to_c[0]);
        while (read(read_pipe, &num, sizeof(int)) == sizeof(int)) {
            if (num % p != 0) {
                write(c_to_c[1], &num, sizeof(num));
            }
        }
        close(read_pipe);
        close(c_to_c[1]);
        wait(0);
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    // exit if there are no arguments
    if (argc != 2) {
        write(2,
            "primes: missing operand\n"
            "Try 'primes n' with n being a positive integer > 2.\n", 77);
        exit(1);
    }
    int n = atoi(argv[1]);
    int p_to_c[2];
    pipe(p_to_c);

    int pid = fork();

    if (pid == 0) { // if child
        close(p_to_c[1]);
        child(p_to_c[0], 2);
    } else {
        close(p_to_c[0]);
        for (int i = 2; i <= n; i++) {
            write(p_to_c[1], &i, sizeof(int));
        }
        close(p_to_c[1]);
        wait(0);
        exit(0);
    }
}
