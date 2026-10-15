OS PROJECT FINAL - CANDIDATE FIX

This version keeps the same project files and names and fixes the prediction/candidate behavior.

Prediction decisions:
- >= 65% : KEEP ACTIVE
- 40% to < 65% : MONITOR
- < 40% : CANDIDATE

An eligible process must also:
- not be a protected Windows/system process
- not be the foreground process
- have at least 60 seconds of tracked background time
- have CPU usage below 1%

A candidate must remain eligible for 3 consecutive automatic checks before suspension:
CANDIDATE 1/3 -> CANDIDATE 2/3 -> CANDIDATE 3/3 -> SUSPENDED

At the low-CPU long-background point, the prediction may reach about 32%. This is intentionally below the 40% candidate threshold, so it will NOT get stuck at 32%. It proceeds through the three candidate checks and then attempts suspension.

Important Windows, security, terminal, and framework processes are protected before suspension is attempted.

Build in MSYS2 UCRT64:
gcc -Wall -o predictive_framework.exe main.c process.c predict.c control.c data.c -lpsapi

Run:
./predictive_framework.exe

Use option 5 for the automatic predictive engine.
Use option 7 to resume all processes managed/suspended by the framework.
