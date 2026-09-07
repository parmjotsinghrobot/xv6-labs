# xv6-labs

This repository contains my work for SOFTENG 370: Operating Systems.

In each assignment, I have been asked to implement many different pieces
of functionality.

## Assignment 1

This assignment implements the below:

### `sleep time`

This user program sleeps for however many seconds are specified.
- I have also implemented prefixes, so that you can specify seconds, minutes,
  hours, or even days.
- If there are multiple arguments, the times are summed up (accounting for
  prefixes)
- Help and version options available (`sleep h`, and `sleep v`).

```
$ sleep h
Usage: sleep NUMBER[SUFFIX]...
  or:  sleep OPTION
Pause for NUMBER seconds, where NUMBER is an integer or floating-point.
SUFFIX may be 's','m','h', or 'd', for seconds, minutes, hours, days.
With multiple arguments, pause for the sum of their values.

      h
         display this help and exit
      v
         output version information and exit
Report bugs to: parmjotsinghrobot@gmail.com
$ sleep v
sleep 1.1
Copyright (C) 2026 Parmjot Singh.

This software is unlicensed.

Written by Parmjot Singh.
$ sleep 10                          # what did you expect?
$
```

### `pingpong`

This user program "plays pingpong" between itself and its two children;
- The parent sends `ping` to each child,
- On receiving each ping, the child prints `<PID>: pong`, and then sends
  `ACK` to its parent.
- The parent waits for each `ACK`, waits for both children to exit, then
  exits.

```
$ pingpong
8: pong
9: pong
$ pingpong
11: pong
12: pong
```

### `getppid`

This system call returns the parent `PID` of the calling process.

### `findppid n`

This user program builds a linear chain of `n` children, and then each child
prints its own `PID`, and its parent's `PID`, all the way to the parent
process, which prints its parent (`init`).

```
$ findppid 5
my ID is 8, my parent ID is 7
my ID is 7, my parent ID is 6
my ID is 6, my parent ID is 5
my ID is 5, my parent ID is 4
my ID is 4, my parent ID is 3
my ID is 3, my parent ID is 2
$ findppid 5
my ID is 15, my parent ID is 14
my ID is 14, my parent ID is 13
my ID is 13, my parent ID is 12
my ID is 12, my parent ID is 11
my ID is 11, my parent ID is 10
my ID is 10, my parent ID is 2
```

### `primes n`

This user program builds a sieve of Eratosthenes to find all prime numbers up to `n`. It does this by forking a child process for each prime number, and passing the remaining numbers to the child process to filter out multiples of that prime number.

```
$ primes 20
2
3
5
7
11
13
17
19
```

### `primes_sum n`

Extends `primes` and computes and prints the sum of two consecutive primes.

```
$ primes_sum 50
5
8
12
18
24
30
36
42
52
60
68
78
84
90
```

### Priority Scheduler

This scheduler replaces the default scheduler with a priority scheduler, where
calling processes can set their priority with `setpri(priority)`, where `1` is
low priority, and `2` is high priority. All processes start as low priority.

#### `setpri(int num)`

System call which changes the priority of the calling process.

#### `getpinfo(struct pstat *)`

This system call returns basic information about each running process,
including its process ID and the amount of time it has spent running at each
priority level, measured in clock ticks.

# Original README

xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6).  xv6 loosely follows the structure and style of v6,
but is implemented for a modern RISC-V multiprocessor using ANSI C.

ACKNOWLEDGMENTS

xv6 is inspired by John Lions's Commentary on UNIX 6th Edition (Peer
to Peer Communications; ISBN: 1-57398-013-7; 1st edition (June 14,
2000)).  See also https://pdos.csail.mit.edu/6.1810/, which provides
pointers to on-line resources for v6.

The following people have made contributions: Russ Cox (context switching,
locking), Cliff Frey (MP), Xiao Yu (MP), Nickolai Zeldovich, and Austin
Clements.

We are also grateful for the bug reports and patches contributed by
Takahiro Aoyagi, Marcelo Arroyo, Silas Boyd-Wickizer, Anton Burtsev,
carlclone, Ian Chen, Dan Cross, Cody Cutler, Mike CAT, Tej Chajed,
Asami Doi,Wenyang Duan, eyalz800, Nelson Elhage, Saar Ettinger, Alice
Ferrazzi, Nathaniel Filardo, flespark, Peter Froehlich, Yakir Goaron,
Shivam Handa, Matt Harvey, Bryan Henry, jaichenhengjie, Jim Huang,
Matúš Jókay, John Jolly, Alexander Kapshuk, Anders Kaseorg, kehao95,
Wolfgang Keller, Jungwoo Kim, Jonathan Kimmitt, Eddie Kohler, Vadim
Kolontsov, Austin Liew, l0stman, Pavan Maddamsetti, Imbar Marinescu,
Yandong Mao, Matan Shabtay, Hitoshi Mitake, Carmi Merimovich, Mark
Morrissey, mtasm, Joel Nider, Hayato Ohhashi, OptimisticSide,
phosphagos, Harry Porter, Greg Price, RayAndrew, Jude Rich, segfault,
Ayan Shafqat, Eldar Sehayek, Yongming Shen, Fumiya Shigemitsu, snoire,
Taojie, Cam Tenny, tyfkda, Warren Toomey, Stephen Tu, Alissa Tung,
Rafael Ubal, Amane Uehara, Pablo Ventura, Xi Wang, WaheedHafez,
Keiichi Watanabe, Lucas Wolf, Nicolas Wolovick, wxdao, Grant Wu, x653,
Jindong Zhang, Icenowy Zheng, ZhUyU1997, and Zou Chang Wei.

The code in the files that constitute xv6 is
Copyright 2006-2024 Frans Kaashoek, Robert Morris, and Russ Cox.

ERROR REPORTS

Please send errors and suggestions to Frans Kaashoek and Robert Morris
(kaashoek,rtm@mit.edu).  The main purpose of xv6 is as a teaching
operating system for MIT's 6.1810, so we are more interested in
simplifications and clarifications than new features.

BUILDING AND RUNNING XV6

You will need a RISC-V "newlib" tool chain from
https://github.com/riscv/riscv-gnu-toolchain, and qemu compiled for
riscv64-softmmu.  Once they are installed, and in your shell
search path, you can run "make qemu".
