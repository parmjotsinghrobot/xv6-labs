#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    // exit if there are no arguments
    if (argc == 1) {
        write(2,
            "sleep: missing operand\n"
            "Try 'sleep h' for more information.\n", 60);
        exit(1);
    }

    if (argv[1][0] == 'h') {
        write(2, 
            "Usage: sleep NUMBER[SUFFIX]...\n"
            "  or:  sleep OPTION\n"
            "Pause for NUMBER seconds, where NUMBER is an integer or floating-point.\n"
            "SUFFIX may be 's','m','h', or 'd', for seconds, minutes, hours, days.\n"
            "With multiple arguments, pause for the sum of their values.\n"
            "\n"
            "      h\n"
            "         display this help and exit\n"
            "      v\n"
            "         output version information and exit\n"
            "Report bugs to: parmjotsinghrobot@gmail.com\n", 396);
        exit(0);
    }

    if (argv[1][0] == 'v') {
        write(2, 
            "sleep 1.1\n"
            "Copyright (C) 2026 Parmjot Singh.\n"
            "\n"
            "This software is unlicensed.\n"
            "\n"
            "Written by Parmjot Singh.\n", 102);
        exit(0);
    }

    // here is my stupid chud sleep function son
    // sleep(atoi(argv[1]) * 10);

    // recalculate the arguments to be in seconds
    int sleepTimes[argc - 1] = {};

    // check if each argument has a suffix, and if so, convert it into seconds
    for (int i = 1; i < argc; i++) {
        // get the last character of each argument string
        int j = 0;
        char suffix = argv[i][j];
        while (suffix != 0) {
            if (argv[i][j + 1] != 0) {
                j++;
                suffix = argv[i][j];
            } else {break;}
        }

        if (suffix == 'h') {
            sleepTimes[i - 1] = atoi(argv[i]) * 3600;
        } else if (suffix == 'm') {
            sleepTimes[i - 1] = atoi(argv[i]) * 60;
        } else (sleepTimes[i - 1] = atoi(argv[i]));
    }

    int sleepTime = 0;

    // sum the arguments to the total sleep time
    for (int i = 0; i < argc - 1; i++) {
        sleepTime += sleepTimes[i];
    }

    // the sleep syscall sleeps for arg * 0.1 sec
    sleep(sleepTime * 10);
    exit(0);
}
