# OSS IPC Manager

*C++, POSIX shared memory, message queues & signals*

---

## Overview
A command-line program that forks up to *N* child `worker` processes, enforcing a concurrency limit of *M*, maintains a shared-memory clock, and sends/receives IPC messages to track each child's lifecycle.

---

## Features

- **Shared-memory clock**  
  Uses `shmget()`/`shmat()` to store a two-word clock (seconds + nanoseconds), incremented by *250 ms / running_processes* each loop.

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
