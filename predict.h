#ifndef PREDICT_H
#define PREDICT_H

double predict_usage(int usage_count,int inactive_seconds,
                    double cpu_percent,int battery);

const char *decision(double probability);

#endif
