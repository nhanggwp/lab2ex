## 📌 Overview

This project implements a **multi-process, multi-threaded** system in C, where:

- **Program A**: acts as the command sender (user interface)
- **Program B**: acts as the thread manager (executor)

Programs communicate via a **bidirectional named pipe (FIFO)** at `/tmp/thread_mgmt_pipe`.

> ⚠️ Note: This version **does not yet implement mutex** for synchronizing shared memory.

---

## ✅ What’s Implemented (vs. Requirements)

| Requirement                                                  | Status                        | Code Handling                                                                 |
|--------------------------------------------------------------|-------------------------------|--------------------------------------------------------------------------------|
| Two separate programs communicating via FIFO                 | ✅ Done                        | `open("/tmp/thread_mgmt_pipe", O_RDWR)` in both A & B                         |
| Manager thread handles commands                              | ✅ Done in Program B           | `pthread_create(..., thread_manager_task, ...)`                               |
| CREATE_THREAD spawns a worker thread                         | ✅ Done                        | `create_worker()` creates worker; worker then spawns `count_to_1000()`        |
| LIST_THREADS prints active thread list                       | ✅ Done (stdout only)          | `list_thread()` prints each active thread                                     |
| KILL_THREAD <id> cancels a thread                            | ✅ Done                        | `kill_thread(int id)` calls `pthread_cancel()`                                |
| Shared data with mutex                                       | ❌ Not yet                     | `struct ShareData` defined, but no mutex used                                 |
| Commands validated before sending                            | ✅ Done                        | `is_valid_command()` and `is_number()` in Program A                           |
| Threads tracked with metadata                                | ✅ Done                        | `ThreadArray[]` stores `id`, `type`, `active`, and `pthread_t`                |


