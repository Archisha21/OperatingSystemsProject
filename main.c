#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "process.h"
#include "predict.h"
#include "control.h"
#include "data.h"

static void clear_input(void)
{
    int c;
    while((c=getchar())!='\n' && c!=EOF);
}

static void show_processes(void)
{
    ProcessInfo list[MAX_PROCESSES];
    int n,i;

    n=get_processes(list,MAX_PROCESSES);

    printf("\nPID\tPROCESS\t\t\tMEMORY\n");
    printf("----------------------------------------------------\n");

    for(i=0;i<n;i++)
        printf("%lu\t%-25s%llu MB\n",
               (unsigned long)list[i].pid,
               list[i].name,
               (unsigned long long)list[i].memory_mb);
}

static int analyze_process(DWORD pid)
{
    ProcessInfo p;
    ProcessInfo list[MAX_PROCESSES];
    int n,i;
    int battery;

    if(!get_process_info(pid,&p))
    {
        printf("Cannot access process %lu.\n",
               (unsigned long)pid);
        return 0;
    }

    printf("\nAnalyzing %s (PID %lu)...\n",
           p.name,(unsigned long)p.pid);

    printf("Measuring CPU usage for 1 second...\n");

    p.cpu_percent=get_cpu_usage(pid);

    battery=get_battery();
    p.battery=battery;

    p.usage_count=1;

    if(p.cpu_percent>=5.0)
        p.usage_count=5;

    if(p.cpu_percent>=20.0)
        p.usage_count=10;

    if(p.cpu_percent<0.5)
        p.inactive_seconds=0;
    else
        p.inactive_seconds=0;

    p.probability=predict_usage(
        p.usage_count,
        p.inactive_seconds,
        p.cpu_percent,
        p.battery
    );

    printf("\nPROCESS INFORMATION\n");
    printf("--------------------\n");
    printf("PID                 : %lu\n",
           (unsigned long)p.pid);

    printf("Name                : %s\n",p.name);

    printf("CPU Usage           : %.2f%%\n",
           p.cpu_percent);

    printf("Memory              : %llu MB\n",
           p.memory_mb);

    if(battery>=0)
        printf("Battery             : %d%%\n",battery);
    else
        printf("Battery             : Not available\n");

    printf("Usage Count         : %d\n",p.usage_count);

    printf("Inactive Time       : %d sec\n",
           p.inactive_seconds);

    printf("Future Usage        : %.2f%%\n",
           p.probability*100);

    printf("Decision            : %s\n",
           decision(p.probability));

    n=get_processes(list,MAX_PROCESSES);

    if(n>0)
    {
        update_history(list,n);

        for(i=0;i<n;i++)
        {
            if(list[i].pid==pid)
            {
                list[i].cpu_percent=p.cpu_percent;
                list[i].battery=p.battery;
                list[i].probability=p.probability;
                list[i].usage_count=p.usage_count;
                list[i].inactive_seconds=p.inactive_seconds;
                break;
            }
        }

        save_history(list,n);
    }

    return 1;
}

static void suspend_menu(void)
{
    DWORD pid;

    printf("Enter PID to suspend: ");

    if(scanf("%lu",&pid)!=1)
    {
        clear_input();
        return;
    }

    if(pid==0 || pid==GetCurrentProcessId())
    {
        printf("Unsafe PID.\n");
        return;
    }

    if(suspend_process(pid))
        printf("Process %lu suspended successfully.\n",
               (unsigned long)pid);
    else
        printf("Could not suspend process %lu.\n",
               (unsigned long)pid);
}

static void resume_menu(void)
{
    DWORD pid;

    printf("Enter PID to resume: ");

    if(scanf("%lu",&pid)!=1)
    {
        clear_input();
        return;
    }

    if(resume_process(pid))
        printf("Process %lu resumed successfully.\n",
               (unsigned long)pid);
    else
        printf("Could not resume process %lu.\n",
               (unsigned long)pid);
}

static void automatic_demo(void)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    char command[]="notepad.exe";

    ZeroMemory(&si,sizeof(si));
    ZeroMemory(&pi,sizeof(pi));

    si.cb=sizeof(si);

    printf("\nStarting safe demo process: notepad.exe\n");

    if(!CreateProcessA(
        NULL,
        command,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi))
    {
        printf("Could not start Notepad.\n");
        return;
    }

    CloseHandle(pi.hThread);

    Sleep(1500);

    printf("Demo PID: %lu\n",(unsigned long)pi.dwProcessId);

    analyze_process(pi.dwProcessId);

    printf("\nApplying predictive decision...\n");

    if(suspend_process(pi.dwProcessId))
    {
        printf("Decision: SUSPEND\n");
        printf("Notepad has been suspended.\n");

        Sleep(3000);

        printf("\nUser requests Notepad again.\n");

        if(resume_process(pi.dwProcessId))
            printf("Decision: RESUME\nNotepad has been resumed.\n");
        else
            printf("Could not resume Notepad.\n");
    }
    else
    {
        printf("Could not suspend demo process.\n");
    }

    Sleep(1000);

    TerminateProcess(pi.hProcess,0);
    WaitForSingleObject(pi.hProcess,2000);
    CloseHandle(pi.hProcess);

    printf("Demo process closed.\n");
}

int main(void)
{
    int choice;
    DWORD pid;

    printf("============================================\n");
    printf(" PREDICTIVE PROCESS FRAMEWORK\n");
    printf(" Windows C Prototype\n");
    printf("============================================\n");

    while(1)
    {
        printf("\n1. Show Running Processes\n");
        printf("2. Analyze Process\n");
        printf("3. Suspend Process\n");
        printf("4. Resume Process\n");
        printf("5. Automatic Prediction Demo\n");
        printf("6. Exit\n");
        printf("\nEnter choice: ");

        if(scanf("%d",&choice)!=1)
        {
            clear_input();
            printf("Invalid input.\n");
            continue;
        }

        switch(choice)
        {
            case 1:
                show_processes();
                break;

            case 2:
                printf("Enter PID: ");
                if(scanf("%lu",&pid)==1)
                    analyze_process(pid);
                else
                    clear_input();
                break;

            case 3:
                suspend_menu();
                break;

            case 4:
                resume_menu();
                break;

            case 5:
                automatic_demo();
                break;

            case 6:
                printf("Exiting...\n");
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}
