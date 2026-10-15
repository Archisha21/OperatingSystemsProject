#include <stdio.h>
#include "data.h"

#define FILE_NAME "usage_history.csv"

void save_usage(ProcessInfo *p)
{
    FILE *f=fopen(FILE_NAME,"a");

    if(!f)
        return;

    fprintf(f,"%lu,%.2f,%llu,%d,%d,%d,%.2f\n",
            p->pid,
            p->cpu_percent,
            (unsigned long long)p->memory_mb,
            p->inactive_seconds,
            p->usage_count,
            p->battery,
            p->probability);

    fclose(f);
}

int get_usage_count(DWORD pid)
{
    FILE *f;
    char line[512];
    unsigned long id;
    double cpu;
    int count=0;

    f=fopen(FILE_NAME,"r");

    if(!f)
        return 0;

    while(fgets(line,sizeof(line),f))
    {
        cpu=0;

        if(sscanf(line,"%lu,%lf",&id,&cpu)==2 && id==pid)
        {
            if(cpu>=1.0)
                count++;
        }
    }

    fclose(f);

    return count;
}

void show_usage_history()
{
    FILE *f;
    char line[512];

    f=fopen(FILE_NAME,"r");

    if(!f)
    {
        printf("\nNo usage history available.\n");
        return;
    }

    printf("\nUSAGE HISTORY\n");
    printf("-------------------------------------------------------------------------------\n");

    while(fgets(line,sizeof(line),f))
        printf("%s",line);

    fclose(f);
}
