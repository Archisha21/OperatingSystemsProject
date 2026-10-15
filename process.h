#ifndef PROCESS_H
#define PROCESS_H
#include <windows.h>
#define MAX_PROCESSES 512
typedef struct { DWORD pid; char name[MAX_PATH]; double cpu_percent; SIZE_T memory_mb; int state; int usage_count; int inactive_seconds; int battery; double probability; } ProcessInfo;
int get_processes(ProcessInfo list[],int max);
int get_process_info(DWORD pid,ProcessInfo *p);
double get_cpu_usage(DWORD pid);
int get_battery();
const char *process_state(DWORD pid);
int get_user_idle_seconds();
int get_process_age_seconds(DWORD pid);
int is_foreground_process(DWORD pid);
int get_background_seconds(DWORD pid);
#endif
