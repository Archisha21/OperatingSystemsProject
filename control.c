#include <stdio.h>
#include <windows.h>
#include "control.h"
#include "process.h"
typedef LONG NTSTATUS;
typedef NTSTATUS (WINAPI *NtSuspendProcessFunc)(HANDLE);
typedef NTSTATUS (WINAPI *NtResumeProcessFunc)(HANDLE);
typedef struct { DWORD pid; int suspended; } SuspendedProcess;
static SuspendedProcess suspended[MAX_PROCESSES];
static int suspended_count=0;
static int find_suspended(DWORD pid){ int i; for(i=0;i<suspended_count;i++) if(suspended[i].pid==pid) return i; return -1; }
int suspend_process(DWORD pid){ HANDLE h; HMODULE ntdll; NtSuspendProcessFunc fn; NTSTATUS status; if(find_suspended(pid)>=0) return 1; h=OpenProcess(PROCESS_SUSPEND_RESUME|PROCESS_QUERY_INFORMATION,FALSE,pid); if(!h) return 0; ntdll=GetModuleHandleA("ntdll.dll"); if(!ntdll){ CloseHandle(h); return 0; } fn=(NtSuspendProcessFunc)GetProcAddress(ntdll,"NtSuspendProcess"); if(!fn){ CloseHandle(h); return 0; } status=fn(h); CloseHandle(h); if(status!=0) return 0; if(suspended_count<MAX_PROCESSES){ suspended[suspended_count].pid=pid; suspended[suspended_count].suspended=1; suspended_count++; } return 1; }
int resume_process(DWORD pid){ HANDLE h; HMODULE ntdll; NtResumeProcessFunc fn; NTSTATUS status; int index,i; h=OpenProcess(PROCESS_SUSPEND_RESUME|PROCESS_QUERY_INFORMATION,FALSE,pid); if(!h) return 0; ntdll=GetModuleHandleA("ntdll.dll"); if(!ntdll){ CloseHandle(h); return 0; } fn=(NtResumeProcessFunc)GetProcAddress(ntdll,"NtResumeProcess"); if(!fn){ CloseHandle(h); return 0; } status=fn(h); CloseHandle(h); if(status!=0) return 0; index=find_suspended(pid); if(index>=0){ for(i=index;i<suspended_count-1;i++) suspended[i]=suspended[i+1]; suspended_count--; } return 1; }
int is_process_suspended(DWORD pid){ return find_suspended(pid)>=0; }
void resume_foreground_processes(){ HWND hwnd; DWORD pid; hwnd=GetForegroundWindow(); if(!hwnd) return; GetWindowThreadProcessId(hwnd,&pid); if(find_suspended(pid)>=0) resume_process(pid); }
void resume_all_suspended_processes(){ DWORD pid; printf("\n========================================\n"); printf(" RESUMING MANAGED PROCESSES\n"); printf("========================================\n"); while(suspended_count>0){ pid=suspended[0].pid; if(resume_process(pid)) printf("PID %lu resumed.\n",pid); else { printf("Could not resume PID %lu.\n",pid); suspended_count--; }} printf("All manageable suspended processes have been processed.\n"); }
