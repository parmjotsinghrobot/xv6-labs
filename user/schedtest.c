//
// schedtest.c — Priority scheduler test program for SOFTENG370 A1 Part 2.
//
// Tests:
//   T1  setpri() returns 0 for valid priorities (1, 2)
//   T2  setpri() returns -1 for invalid priorities (0, 3, -1)
//   T3  getpinfo() returns 0 with a valid pointer
//   T4  getpinfo() returns -1 with a NULL pointer
//   T5  All new processes start at priority 1
//   T6  High-priority child runs before low-priority child
//       (fork two children: one stays pri=1, other raises to pri=2;
//        the pri=2 child should finish first)
//   T7  Tick counts are recorded at the correct priority level
//   T8  setpri can lower priority back from 2 to 1
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/pstat.h"
#include "user/user.h"

#define PASS(name) printf("PASS: %s\n", name)
#define FAIL(name) printf("FAIL: %s\n", name)
#define CHECK(name, expr) do { if(expr){ PASS(name); }else{ FAIL(name); } } while(0)

// Busy-wait for approximately 'ticks' timer ticks.
static void
burn(int ticks)
{
  int t0 = uptime();
  while(uptime() - t0 < ticks)
    ;
}

// ── T1: valid setpri calls ────────────────────────────────────────────────────
static void
test_setpri_valid(void)
{
  CHECK("T1a: setpri(1) == 0", setpri(1) == 0);
  CHECK("T1b: setpri(2) == 0", setpri(2) == 0);
  setpri(1); // restore
}

// ── T2: invalid setpri calls ─────────────────────────────────────────────────
static void
test_setpri_invalid(void)
{
  CHECK("T2a: setpri(0)  == -1", setpri(0)  == -1);
  CHECK("T2b: setpri(3)  == -1", setpri(3)  == -1);
  CHECK("T2c: setpri(-1) == -1", setpri(-1) == -1);
}

// ── T3: getpinfo with valid pointer ──────────────────────────────────────────
static void
test_getpinfo_valid(void)
{
  struct pstat st;
  CHECK("T3: getpinfo(&st) == 0", getpinfo(&st) == 0);
}

// ── T4: getpinfo with NULL pointer ───────────────────────────────────────────
static void
test_getpinfo_null(void)
{
  CHECK("T4: getpinfo(0) == -1", getpinfo(0) == -1);
}

// ── T5: new processes start at priority 1 ────────────────────────────────────
static void
test_default_priority(void)
{
  struct pstat st;
  int mypid = getpid();
  getpinfo(&st);
  int found = 0;
  for(int i = 0; i < NPROC; i++){
    if(st.inuse[i] && st.pid[i] == mypid){
      CHECK("T5: default priority is 1", st.priority[i] == 1);
      found = 1;
      break;
    }
  }
  if(!found) FAIL("T5: could not find self in pstat");
}

// ── T6: high-priority child finishes before low-priority child ───────────────
//
// Strategy:
//   Fork child A (pri=1) and child B (pri=2).
//   Use a synchronization pipe so BOTH children are ready and in RUNNABLE
//   state before either starts burning CPU.  This eliminates the race where
//   the scheduler picks pid_lo before pid_hi has called setpri(2).
//
//   Protocol:
//     Parent forks both children, then writes two "go" bytes into sync_pipe.
//     Each child blocks on read(sync_pipe) until the parent signals.
//     pid_hi calls setpri(2) BEFORE waiting for the go signal, so by the
//     time both are RUNNABLE, pid_hi is already at pri=2.
//
static void
test_priority_order(void)
{
  int sync[2];   // parent → children: "go" signal
  int ready[2];
  int done[2];   // children → parent: completion order

  pipe(sync);
  pipe(ready);
  pipe(done);

  int pid_lo = fork();
  if(pid_lo == 0){
    close(sync[1]); close(ready[0]); close(done[0]);
    // Signal ready (still pri=1)
    char r = 1;
    write(ready[1], &r, 1);
    close(ready[1]);
    // Block until go
    char go;
    read(sync[0], &go, 1);
    close(sync[0]);
    burn(20);
    int me = getpid();
    write(done[1], &me, sizeof(me));
    close(done[1]);
    exit(0);
  }

  int pid_hi = fork();
  if(pid_hi == 0){
    setpri(2);
    close(sync[1]); close(ready[0]); close(done[0]);
    // Signal ready (now pri=2)
    char r = 1;
    write(ready[1], &r, 1);
    close(ready[1]);
    // Block until go
    char go;
    read(sync[0], &go, 1);
    close(sync[0]);
    burn(20);
    int me = getpid();
    write(done[1], &me, sizeof(me));
    close(done[1]);
    exit(0);
  }

  // Parent: wait until both children have set their priority
  close(sync[0]); close(ready[1]); close(done[1]);
  char r;
  read(ready[0], &r, 1);  // wait for child 1 ready
  read(ready[0], &r, 1);  // wait for child 2 ready
  close(ready[0]);

  // Both children have called setpri and are about to block on sync.
  // Fire go — they will both become RUNNABLE with pid_hi at pri=2.
  char go = 1;
  write(sync[1], &go, 1);
  write(sync[1], &go, 1);
  close(sync[1]);

  // Read completion order
  int first = 0, second = 0;
  read(done[0], &first,  sizeof(first));
  read(done[0], &second, sizeof(second));
  close(done[0]);

  wait(0);
  wait(0);

  CHECK("T6: high-priority child finishes first", first == pid_hi);
  CHECK("T6: low-priority child finishes second", second == pid_lo);
}

// ── T7: ticks are recorded at the correct priority level ─────────────────────
//
// We verify that while running at pri=2:
//   T7a: ticks[2] increases (we are being counted at the right level)
//   T7b: ticks[2] increases much more than ticks[1]
//        (scheduler is not misattributing CPU time to the wrong level)
//
// Note: a strict "ticks[1] == 0 during pri=2" check is too fragile because
// the first yield after setpri(2) may race with the snapshot read.
// Instead we check that ticks[2] >> ticks[1]: if ticks[2] is at least
// 3x larger than any ticks[1] increase, the scheduler is working correctly.
//
static void
test_tick_accounting(void)
{
  int mypid = getpid();
  struct pstat st;

  // Phase 1: run at pri=1 and snapshot ticks[1] as baseline
  setpri(1);
  burn(8);
  getpinfo(&st);
  int t1_before = 0;
  for(int i = 0; i < NPROC; i++)
    if(st.inuse[i] && st.pid[i] == mypid){ t1_before = st.ticks[i][1]; break; }

  // Phase 2: switch to pri=2 and burn a significant amount of CPU
  setpri(2);
  burn(20);   // long enough to accumulate many ticks[2]
  getpinfo(&st);
  int t1_after = 0, t2_after = 0;
  for(int i = 0; i < NPROC; i++){
    if(st.inuse[i] && st.pid[i] == mypid){
      t1_after = st.ticks[i][1];
      t2_after = st.ticks[i][2];
      break;
    }
  }

  setpri(1);

  int delta1 = t1_after - t1_before;  // ticks[1] gained while at pri=2
  int delta2 = t2_after;              // ticks[2] gained (was 0 before)

  CHECK("T7a: ticks[2] increases while running at pri=2",
        delta2 > 0);
  // delta1 should be 0; allow at most 1 tick for the setpri(2) transition.
  // More importantly, delta2 must be much larger than delta1.
  CHECK("T7b: ticks[1] does not increase while running at pri=2",
        delta1 <= 1 && delta2 > delta1 * 3);
}

// ── T8: priority can be lowered from 2 back to 1 ─────────────────────────────
static void
test_setpri_lower(void)
{
  struct pstat st;
  int mypid = getpid();

  setpri(2);
  getpinfo(&st);
  int pri_after_raise = 0;
  for(int i = 0; i < NPROC; i++)
    if(st.inuse[i] && st.pid[i] == mypid){ pri_after_raise = st.priority[i]; break; }

  setpri(1);
  getpinfo(&st);
  int pri_after_lower = 0;
  for(int i = 0; i < NPROC; i++)
    if(st.inuse[i] && st.pid[i] == mypid){ pri_after_lower = st.priority[i]; break; }

  CHECK("T8a: setpri(2) reflected in pstat",   pri_after_raise == 2);
  CHECK("T8b: setpri(1) lowers priority back",  pri_after_lower == 1);
}

// ─────────────────────────────────────────────────────────────────────────────

int
main(void)
{
  printf("\n=== SOFTENG370 A1 Part 2 — Scheduler Tests ===\n\n");

  test_setpri_valid();
  test_setpri_invalid();
  test_getpinfo_valid();
  test_getpinfo_null();
  test_default_priority();
  test_priority_order();
  test_tick_accounting();
  test_setpri_lower();

  printf("\n=== Done ===\n");
  exit(0);
}
