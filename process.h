#ifndef PROCESS_H
#define PROCESS_H

#include <windows.h>

#define MAX_PROCESSES 512

typedef struct {
    DWORD pid;
    char name[MAX_PATH];
    double cpu_percent;
    unsigned long long memory_mb;
    int state;
    int usage_count;
    int inactive_seconds;
    int battery;
    double probability;
    ULONGLONG first_seen;
    ULONGLONG last_active;
} ProcessInfo;

int get_processes(ProcessInfo list[],int max);
int get_process_info(DWORD pid,ProcessInfo *p);
int get_battery(void);
double get_cpu_usage(DWORD pid);
const char *process_state(DWORD pid);

#endif