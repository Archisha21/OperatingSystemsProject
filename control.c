#include "control.h"
#include <windows.h>

typedef LONG NTSTATUS;

typedef NTSTATUS (NTAPI *NtSuspendProcessFunc)(HANDLE);
typedef NTSTATUS (NTAPI *NtResumeProcessFunc)(HANDLE);

int suspend_process(DWORD pid)
{
    HMODULE ntdll;
    NtSuspendProcessFunc fn;
    HANDLE h;
    NTSTATUS status;

    ntdll=GetModuleHandleA("ntdll.dll");
    if(!ntdll) return 0;

    fn=(NtSuspendProcessFunc)GetProcAddress(ntdll,"NtSuspendProcess");
    if(!fn) return 0;

    h=OpenProcess(PROCESS_SUSPEND_RESUME,FALSE,pid);
    if(!h) return 0;

    status=fn(h);
    CloseHandle(h);

    return status==0;
}

int resume_process(DWORD pid)
{
    HMODULE ntdll;
    NtResumeProcessFunc fn;
    HANDLE h;
    NTSTATUS status;

    ntdll=GetModuleHandleA("ntdll.dll");
    if(!ntdll) return 0;

    fn=(NtResumeProcessFunc)GetProcAddress(ntdll,"NtResumeProcess");
    if(!fn) return 0;

    h=OpenProcess(PROCESS_SUSPEND_RESUME,FALSE,pid);
    if(!h) return 0;

    status=fn(h);
    CloseHandle(h);

    return status==0;
}
