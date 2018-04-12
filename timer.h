#ifndef TIMER_H
#define TIMER_H

#define MAXTIMERS 20


typedef int (*Operation)();

typedef struct timers
{
    unsigned long time;
    Operation opt;
}timers_t;

void jiffies();
int timer(int time,void *functioncall);



#endif
