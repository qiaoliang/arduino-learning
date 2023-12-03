#ifndef FEEDING_H
#define FEEDING_H

#include <Ticker.h>
#include "MyServo.h"
#include "SysConfig.h"

extern SysConfig sysCfg;
extern int targetDegree ;

Ticker ticktick;

int feed_count = 0;
const int RIGHTNOW =1;
const int MINS = 60;
const int HOURS = 60 * 60;

void feed();
void stop();
void waiting();
void go(int value)
{
    targetDegree = value;
}
void FeedNow(){
    feed_count = 0;
    ticktick.attach(RIGHTNOW, feed);
}
void feed()
{
    feed_count++;
    go(179);
    ticktick.attach(sysCfg.getSpeed().toInt(), stop);
}
void stop()
{
    go(1);
    if (feed_count < sysCfg.getCount().toInt())
    {
        ticktick.attach(sysCfg.getSpeed().toInt(), feed);
    }
    else
    {
        ticktick.attach(sysCfg.getInterval().toInt()*HOURS, waiting);
    }
}

void waiting()
{
    feed_count = 0;
    ticktick.attach(sysCfg.getSpeed().toInt(), feed);
}

int currentDegree;
int targetDegree;

/**
 * @brief 如果没有达到指定角度，则继续旋转. 放在Loop()中.
 */
void keepRunning()
{
    currentDegree = S.read();

    if (currentDegree < targetDegree)
    {
        S.write(toPwm(currentDegree + 1));
        delay(2);
    }
    else if (currentDegree > targetDegree)
    {
        S.write(toPwm(currentDegree - 1));
        delay(2);
    }else{

    }
}

void Feeding_Init()
{
    // 每30秒 转动一次servo
    ticktick.attach(sysCfg.getSpeed().toInt(), feed);
}
#endif