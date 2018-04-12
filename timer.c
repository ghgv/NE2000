#include "timer.h"
#include <signal.h>
#include <stdio.h>


unsigned long click=0;
timers_t arrayoftimers[MAXTIMERS];

void jiffies(){
  int i;
  click++;

  for(i=0;i<MAXTIMERS;i++)
    {
      if(click>arrayoftimers[i].time && arrayoftimers[i].opt!=NULL ){
        arrayoftimers[i].opt();
        fflush(stdout);
        arrayoftimers[i].opt=NULL;
      }

    }
  alarm(1);
  signal(SIGALRM, jiffies);
}

int timer(int time,void *functioncall){

  int i;

  if(time<=0)
   return -1;

  for(i=0;i<MAXTIMERS;i++)
    {
      if(arrayoftimers[i].opt==NULL)
        {
          arrayoftimers[i].time=click+time;//time in seconds
          arrayoftimers[i].opt=functioncall;
          printf("Timer %i set. Calling %s\n",i,functioncall);
          return i;
        }
    }
 return -1; //Not available  time slot

}
