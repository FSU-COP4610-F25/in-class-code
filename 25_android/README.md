1. Open **two** terminal windows.

2. In the first terminal, run:
   ```bash
   ./flock-demo a b
````

3. In the second terminal, run:

   ```bash
   ./flock-demo b a
   ```

   Now the two processes hold the locks in opposite order and enter a deadlock.

4. Open a **third** terminal and check the running processes:

   ```bash
   ps ax | grep flock-demo
   ```

5. Kill one of the deadlocked processes (replace `PID` with the actual process ID you see):

   ```bash
   kill -9 PID
   ```

6. Observe that after one process is killed, the other `flock-demo` process is able to acquire the remaining lock and continue running. This shows that even though the two processes were in a deadlock, terminating one of them allows the other to make progress.


