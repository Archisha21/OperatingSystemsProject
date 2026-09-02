#ifndef CONTROL_H
#define CONTROL_H

#include <windows.h>

int suspend_process(DWORD pid);
int resume_process(DWORD pid);

#endif
