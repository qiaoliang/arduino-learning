#include <TimerOne.h>

extern void trace_check();
int count =1;
void timerIRS()
{

  (++count)%=100;

//  if(count ==0)
    //trace_check();
}

void Timer1_init(){
  Timer1.initialize(1000); //1ms
  Timer1.attachInterrupt(timerIRS);
}