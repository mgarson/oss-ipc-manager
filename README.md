# OSS IPC Manager

*C++, POSIX shared memory, message queues & signals*

---

## Overview
A command-line program that forks up to *N* child `worker` processes, enforcing a concurrency limit of *M*, maintains a shared-memory clock, and sends/receives IPC messages to track each child's lifecycle.

---

## Features

- **Shared-memory clock**  
  Uses `shmget()`/`shmat()` to store a two-word clock (seconds + nanoseconds), incremented by `250000000 ns/running_processes` each loop.

- **Process forking**  
  Spawns each `worker` child via `fork()` + `execvp()`, up to *N* total processes (set by `-n N`, default: 1).

- **Concurrency control**  
  Ensures no more than *M* workers run simultaneously, (set by `-s M`, default: 1); parent reaps children with `waitpid(..., WNOHANG)`.

- **Configurable time limits**  
  Children run for a random duration ≤ *T* seconds (set by `-t T`, default: 1), passed as an argument to `worker`.

- **Fork interval**  
  Attempts a new fork every *I* ns (set by `-i I`, default: 0); too-early forks emit an error and retry.

- **IPC messaging**  
  Uses `msgget()`/`msgsnd()`/`msgrcv()` to send "still working" or "finished" flags to/from each child, tracking `messagesSent`.

- **Optional logging**  
  Pass `-f` to write console output to `ossLog.txt` in real time.

- **Real-time safety**  
  Installs `SIGALRM` via `alarm(60)` to kill any remaining children after 60 s and exit cleanly.

- **PCB table printing**  
  Every 0.5 s of simulated time, prints a table of occupied entries, PIDs, start times, and messages sent.

---

## Build & Run

```bash
# 1. Clone
git clone git@github.com:mgarson/oss-ipc-manager.git
cd oss-ipc-manager

# 2. Build
make

# 3. Usage
./oss [-h] [-n N] [-s M] [-t T] [-i I] [-f]

# Options
#   -h         Show help
#   -n N       Total child processes (default: 1)
#   -s M       Max simultaneous workers (default: 1)
#   -t T       Upper bound (s) for child run time (default: 1)
#   -i I       Min interval (ns) between forks (default: 0)
#   -f         Log output to ossLog.txt

# Examples
./oss -n 5 -s 2 -t 3 -i 100000000  # Launch up to 5 workers, 2 at a time; each runs ≤3 s; forks ≥0.1 s apart
./oss -n 10 -f                     # Launch 10 workers with logging enabled
```

---

## Technical Highlights

- **Adaptive clock increment**  
  Derives nanosecond increment = `250000000 ns/running_processes` to simulate variable CPU load

- **Robust IPC**  
  Creates a dedicated message queue via `msget()` `(ftok"msgq.txt", 1)` and uses typed buffers to coordinate parent/child status.

- **Dynamic PCB Management**  
  Allocates `processTable[N]` entries, tracking `occupied`, `pid`, `startSeconds`, `startNano`, and `messagesSent`.

- **Graceful termination**  
  Uses `alarm(60)` + `SIGALRM` handler to forcibly kill orphans, then removes shared memory and message queue with `shmctl()`/`msgctl()`

- **Flexible logging**  
  Optional `-f` flag opens `ossLog.txt` for synchronous console + file output, capturing lifecycle events and IPC traces.
