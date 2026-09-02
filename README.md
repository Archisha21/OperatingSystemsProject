# Predictive Process Framework - Windows C Version

## Requirements

- Windows 10/11
- MinGW-w64 GCC
- VS Code
- Make

The project uses Windows APIs and `psapi`.

## Compile

Open the VS Code terminal in this folder:

    mingw32-make

If your installation provides `make` instead:

    make

## Run

    .\predictive_framework.exe

or:

    mingw32-make run

## Menu

1. Show Running Processes
2. Analyze Process
3. Suspend Process
4. Resume Process
5. Automatic Prediction Demo
6. Exit

## Review 2 demo

Use option 5.

The program launches Notepad as a demonstration process, analyzes it, calculates a future-usage probability, suspends it, waits three seconds, resumes it, and then closes the demo process.

This avoids choosing a random critical Windows process during the presentation.

## Architecture

Monitor Process
-> Collect Data
-> Predict Future Usage
-> Decision
-> Suspend
-> Resume
-> Evaluate

## Notes

The prediction module currently uses a weighted scoring prototype rather than a trained ML model. The scoring module can later be replaced by an actual ML model.

Process suspension uses the native Windows `NtSuspendProcess` and `NtResumeProcess` functions dynamically from ntdll.dll.

Some Windows processes may reject access depending on permissions. Do not experiment with system-critical processes.
