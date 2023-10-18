#include <TimerOne.h>

void timerIRS()
{
    //Serial.println("哈哈");
}

void Timer1_init(){
  Timer1.initialize(1000); //1ms
  Timer1.attachInterrupt(timerIRS);
}