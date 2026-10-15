#ifndef CONTROL_H
#define CONTROL_H
#include <windows.h>
int suspend_process(DWORD pid);
int resume_process(DWORD pid);
int is_process_suspended(DWORD pid);
void resume_foreground_processes();
void resume_all_suspended_processes();
#endif
