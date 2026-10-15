#ifndef DATA_H
#define DATA_H

#include "process.h"

void save_usage(ProcessInfo *p);
int get_usage_count(DWORD pid);
void show_usage_history();

#endif
