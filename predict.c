#include <string.h>
#include "predict.h"
#include "process.h"

double predict_usage(
    int usage_count,
    int inactive_seconds,
    double cpu_percent,
    int battery,
    SIZE_T memory_mb)
{
    double score=0.80;

    if(usage_count>=20)
        score+=0.10;
    else if(usage_count>=10)
        score+=0.06;
    else if(usage_count>=5)
        score+=0.03;

    if(cpu_percent>=20)
        score+=0.15;
    else if(cpu_percent>=5)
        score+=0.10;
    else if(cpu_percent>=1)
        score+=0.05;
    else
        score-=0.03;

    if(inactive_seconds<20)
        score+=0.05;
    else if(inactive_seconds<40)
        score-=0.05;
    else if(inactive_seconds<60)
        score-=0.20;
    else
        score-=0.35;

    if(cpu_percent<1.0 && inactive_seconds>=30)
        score-=0.10;

    if(memory_mb>=1000)
        score-=0.03;
    else if(memory_mb>=500)
        score-=0.01;

    if(battery>=0)
    {
        if(battery<20)
            score-=0.08;
        else if(battery<40)
            score-=0.04;
    }

    if(score<0)
        score=0;

    if(score>1)
        score=1;

    return score;
}

const char *decision(double probability)
{
    if(probability>=0.65)
        return "KEEP ACTIVE";

    if(probability>=0.40)
        return "MONITOR";

    return "CANDIDATE";
}

int is_safe_to_manage(DWORD pid,const char *name)
{
    const char *protected[]={
        "System",
        "System Idle Process",
        "Secure System",
        "Registry",

        "smss.exe",
        "csrss.exe",
        "wininit.exe",
        "services.exe",
        "lsass.exe",
        "winlogon.exe",
        "svchost.exe",
        "dwm.exe",
        "explorer.exe",

        "fontdrvhost.exe",
        "Memory Compression",

        "MsMpEng.exe",
        "MpDefenderCoreService.exe",
        "SecurityHealthService.exe",

        "SearchIndexer.exe",
        "SearchProtocolHost.exe",
        "SearchFilterHost.exe",
        "SearchHost.exe",

        "TrustedInstaller.exe",
        "TiWorker.exe",
        "MoUsoCoreWorker.exe",

        "RuntimeBroker.exe",
        "StartMenuExperienceHost.exe",
        "ShellExperienceHost.exe",
        "TextInputHost.exe",
        "sihost.exe",
        "ctfmon.exe",
        "ApplicationFrameHost.exe",

        "conhost.exe",
        "cmd.exe",
        "powershell.exe",
        "WindowsTerminal.exe",
        "bash.exe",
        "mintty.exe",

        "wslservice.exe",

        "audiodg.exe",
        "WmiPrvSE.exe",
        "dllhost.exe",
        "backgroundTaskHost.exe",

        "spoolsv.exe",
        "WUDFHost.exe",
        "WUDFCompanionHost.exe",

        "NisSrv.exe",
        "wlanext.exe",

        "predictive_framework.exe",
        "predictive_framework_windows.exe"
    };

    int i;
    int n=sizeof(protected)/sizeof(protected[0]);

    if(pid<100)
        return 0;

    for(i=0;i<n;i++)
    {
        if(_stricmp(name,protected[i])==0)
            return 0;
    }

    return 1;
}

int should_suspend(
    DWORD pid,
    const char *name,
    double probability,
    int background_seconds,
    double cpu_percent)
{
    if(!is_safe_to_manage(pid,name))
        return 0;

    if(is_foreground_process(pid))
        return 0;

    if(background_seconds<60)
        return 0;

    if(cpu_percent>=1.0)
        return 0;

    if(probability>=0.40)
        return 0;

    return 1;
}
