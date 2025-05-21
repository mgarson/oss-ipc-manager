# OSS IPC Manager

*C++, POSIX shared memory, message queues & signals*

---

## Overview
A command-line program that forks up to *N* child `worker` processes, enforcing a concurrency limit of *M*, maintains a shared-memory clock, and sends/receives IPC messages to track each child's lifecycle.

---

## Features

