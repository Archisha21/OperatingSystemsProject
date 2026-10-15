#ifndef PREDICT_H
#define PREDICT_H
#include <windows.h>
double predict_usage(int usage_count,int inactive_seconds,double cpu_percent,int battery,SIZE_T memory_mb);
const char *decision(double probability);
int is_safe_to_manage(DWORD pid,const char *name);
int should_suspend(DWORD pid,const char *name,double probability,int background_seconds,double cpu_percent);
#endif
