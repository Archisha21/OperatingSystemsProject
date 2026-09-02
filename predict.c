#include "predict.h"

double predict_usage(int usage_count,
                     int inactive_seconds,
                     double cpu_percent,
                     int battery)
{
    double score=0.50;

    if(usage_count>=10)
        score+=0.20;
    else if(usage_count>=5)
        score+=0.10;
    else if(usage_count<=1)
        score-=0.10;

    if(inactive_seconds<30)
        score+=0.20;
    else if(inactive_seconds<120)
        score+=0.05;
    else if(inactive_seconds<300)
        score-=0.10;
    else
        score-=0.25;

    if(cpu_percent>20.0)
        score+=0.20;
    else if(cpu_percent>5.0)
        score+=0.10;
    else if(cpu_percent<0.5)
        score-=0.10;

    if(battery>=0)
    {
        if(battery<20)
            score-=0.10;
        else if(battery<40)
            score-=0.05;
    }

    if(score<0.0)
        score=0.0;

    if(score>1.0)
        score=1.0;

    return score;
}

const char *decision(double probability)
{
    if(probability>=0.65)
        return "KEEP RUNNING";

    if(probability>=0.40)
        return "MONITOR";

    return "SUSPEND";
}