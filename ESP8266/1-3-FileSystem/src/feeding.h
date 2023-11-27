#ifndef FEEDING_H
#define FEEDING_H

#include <Ticker.h>
#include "MyServo.h"

extern int targetDegree ;

const int TotalCount = 3;  // 每次投食都会转三次.
const int SPEED = 5; // 每隔5秒转动一次投食器
const int INTERVAL = 60;  // 每隔60秒喂一次鱼


Ticker ticktick;

int count = 0;


void feed();
void stop();
void waiting();
void go(int value)
{
    targetDegree = value;
}
void feed()
{
    count++;
    go(179);
    ticktick.attach(SPEED, stop);
}
void stop()
{
    go(1);
    if (count != 3)
    {
        ticktick.attach(SPEED, feed);
    }
    else
    {
        ticktick.attach(INTERVAL, waiting);
    }
}

void waiting()
{
    count = 0;
    ticktick.attach(SPEED, feed);
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
        delay(1);
    }
    else if (currentDegree > targetDegree)
    {
        S.write(toPwm(currentDegree - 1));
        delay(1);
    }
}

void Feeding_Init()
{
    // 每30秒 转动一次servo
    ticktick.attach(SPEED, feed);
}
#endif