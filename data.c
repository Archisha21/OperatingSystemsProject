#include "data.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    DWORD pid;
    int usage_count;
    int inactive_seconds;
} History;

#define MAX_HISTORY 1024

static History history[MAX_HISTORY];
static int history_count=0;

void update_history(ProcessInfo list[],int count)
{
    int i,j;

    for(i=0;i<count;i++)
    {
        int found=-1;

        for(j=0;j<history_count;j++)
        {
            if(history[j].pid==list[i].pid)
            {
                found=j;
                break;
            }
        }

        if(found==-1 && history_count<MAX_HISTORY)
        {
            found=history_count++;
            history[found].pid=list[i].pid;
            history[found].usage_count=0;
            history[found].inactive_seconds=0;
        }

        if(found>=0)
        {
            if(list[i].cpu_percent>1.0)
                history[found].usage_count++;
            else
                history[found].inactive_seconds+=2;

            list[i].usage_count=history[found].usage_count;
            list[i].inactive_seconds=history[found].inactive_seconds;
        }
    }
}

void save_history(ProcessInfo list[],int count)
{
    FILE *fp;
    int i;

    fp=fopen("process_history.csv","w");
    if(!fp) return;

    fprintf(fp,"pid,name,usage_count,inactive_seconds,cpu_percent,"
               "memory_mb,battery,probability,decision\n");

    for(i=0;i<count;i++)
    {
        fprintf(fp,"%lu,%s,%d,%d,%.2f,%llu,%d,%.3f,%s\n",
                (unsigned long)list[i].pid,
                list[i].name,
                list[i].usage_count,
                list[i].inactive_seconds,
                list[i].cpu_percent,
                (unsigned long long)list[i].memory_mb,
                list[i].battery,
                list[i].probability,
                list[i].probability>=0.40?"KEEP":"SUSPEND");
    }

    fclose(fp);
}
