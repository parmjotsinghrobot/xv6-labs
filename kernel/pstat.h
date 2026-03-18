#ifndef _PSTAT_H_
#define _PSTAT_H_

#include "param.h"

// Per-process info returned by getpinfo().
// inuse[i] == 1  means proc slot i is occupied.
// pid[i]         is the process ID.
// priority[i]    is the current priority (1 or 2).
// ticks[i][1]    is CPU ticks spent at priority 1.
// ticks[i][2]    is CPU ticks spent at priority 2.
struct pstat {
  int inuse[NPROC];
  int pid[NPROC];
  int priority[NPROC];
  int ticks[NPROC][3];   // index 0 unused; use [1] and [2]
};

#endif // _PSTAT_H_
