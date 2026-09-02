#include "process.h"
#include <tlhelp32.h>
#include <psapi.h>
#include <stdio.h>
#include <string.h>

static ULONGLONG filetime_to_ull(FILETIME ft)
{
    ULARGE_INTEGER x;
    x.LowPart=ft.dwLowDateTime;
    x.HighPart=ft.dwHighDateTime;
    return x.QuadPart;
}

double get_cpu_usage(DWORD pid)
{
    HANDLE h;
    FILETIME create,exit,kernel1,user1,kernel2,user2;
    ULONGLONG k1,u1,k2,u2;
    SYSTEM_INFO si;
    double process_time;

    h=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid);

    if(!h)
        return 0.0;

    if(!GetProcessTimes(h,&create,&exit,&kernel1,&user1))
    {
        CloseHandle(h);
        return 0.0;
    }

    Sleep(1000);

    if(!GetProcessTimes(h,&create,&exit,&kernel2,&user2))
    {
        CloseHandle(h);
        return 0.0;
    }

    k1=filetime_to_ull(kernel1);
    u1=filetime_to_ull(user1);
    k2=filetime_to_ull(kernel2);
    u2=filetime_to_ull(user2);

    process_time=(double)((k2-k1)+(u2-u1))/10000000.0;

    GetSystemInfo(&si);

    if(si.dwNumberOfProcessors==0)
        si.dwNumberOfProcessors=1;

    CloseHandle(h);

    return (process_time*100.0)/(1.0*si.dwNumberOfProcessors);
}

int get_process_info(DWORD pid,ProcessInfo *p)
{
    HANDLE h;
    PROCESS_MEMORY_COUNTERS pmc;
    FILETIME create,exit,kernel,user;
    char name[MAX_PATH];

    memset(p,0,sizeof(ProcessInfo));

    p->pid=pid;
    p->first_seen=GetTickCount64();
    p->last_active=GetTickCount64();

    h=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid);

    if(!h)
        return 0;

    memset(name,0,sizeof(name));

    if(GetModuleBaseNameA(h,NULL,name,sizeof(name)))
        strncpy(p->name,name,sizeof(p->name)-1);
    else
        snprintf(p->name,sizeof(p->name),"PID_%lu",
                 (unsigned long)pid);

    if(GetProcessMemoryInfo(h,&pmc,sizeof(pmc)))
    {
        p->memory_mb=
            (unsigned long long)pmc.WorkingSetSize/(1024ULL*1024ULL);
    }

    if(GetProcessTimes(h,&create,&exit,&kernel,&user))
    {
        ULONGLONG creation=filetime_to_ull(create);
        ULONGLONG now=filetime_to_ull((FILETIME){
            .dwLowDateTime=0,
            .dwHighDateTime=0
        });

        (void)creation;
        (void)now;
    }

    p->cpu_percent=get_cpu_usage(pid);
    p->state=1;

    CloseHandle(h);

    return 1;
}

int get_processes(ProcessInfo list[],int max)
{
    HANDLE snapshot;
    PROCESSENTRY32 entry;
    int count=0;

    snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

    if(snapshot==INVALID_HANDLE_VALUE)
        return 0;

    memset(&entry,0,sizeof(entry));
    entry.dwSize=sizeof(entry);

    if(Process32First(snapshot,&entry))
    {
        do
        {
            HANDLE h;
            PROCESS_MEMORY_COUNTERS pmc;

            if(count>=max)
                break;

            memset(&list[count],0,sizeof(ProcessInfo));

            list[count].pid=entry.th32ProcessID;

            strncpy(list[count].name,
                    entry.szExeFile,
                    sizeof(list[count].name)-1);

            list[count].name[sizeof(list[count].name)-1]='\0';

            list[count].state=1;
            list[count].first_seen=GetTickCount64();
            list[count].last_active=GetTickCount64();

            h=OpenProcess(
                PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,
                FALSE,
                entry.th32ProcessID
            );

            if(h)
            {
                if(GetProcessMemoryInfo(h,&pmc,sizeof(pmc)))
                {
                    list[count].memory_mb=
                        (unsigned long long)pmc.WorkingSetSize/
                        (1024ULL*1024ULL);
                }

                CloseHandle(h);
            }

            count++;

        }while(Process32Next(snapshot,&entry));
    }

    CloseHandle(snapshot);

    return count;
}

int get_battery(void)
{
    SYSTEM_POWER_STATUS s;

    if(!GetSystemPowerStatus(&s))
        return -1;

    if(s.BatteryLifePercent==255)
        return -1;

    return (int)s.BatteryLifePercent;
}

const char *process_state(DWORD pid)
{
    HANDLE h=OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,pid);

    if(!h)
        return "UNKNOWN";

    CloseHandle(h);

    return "RUNNING";
}