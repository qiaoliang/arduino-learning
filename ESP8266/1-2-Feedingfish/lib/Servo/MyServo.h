#ifndef MYSERVO_H
#define MYSERVO_H

#include <Arduino.h>
#include <Servo.h>

// 定义 0-180 角度值转为对应高电平脉冲信号时间宽度（脉冲宽度）int maxPos = 180;
int minPos = 0;
int maxPos = 180;
float factor = 11.11; // 每转一度对应的脉冲宽度，11.11=(2500-500)/180度

Servo S;

/**
 * @brief 把 0-180 角度值转为对应高电平脉冲信号时间宽度（脉冲宽度）
 *
 * @param degree 目标角度值
 * @return int 输入角度所对应的脉冲宽度，单位是微秒。大于 360时，直接返回，或 500 ， 2500。
 */
int toPwm(float degree)
{ // 返回  把角度值转换为维持高电平的时间长度
    unsigned long pulseWidth;
    if (degree < 360.0)
    { // 值少于360视为角度,否则，入参被视为时间长度，单位为微秒
        unsigned long pulseWidth = degree * factor;
        if ((float)(pulseWidth - (int)pulseWidth) >= 0.45) // 四舍五入
            degree = degree + 1.0;
        return 500 + (int)pulseWidth; // 500 对应着起点，即 0 度。
    }
    else
    {
        pulseWidth = constrain((int)degree, 500, 2500); // 取值500~2500间,9克舵机都对应这个数值。
        return pulseWidth;
    }
}

void Servo_Init()
{
    S.attach(D2, 500, 2500); // 绑定针脚,设置信号脉冲宽度范围, 以便精确控制转到的角度;
    minPos = toPwm((float)minPos);
    maxPos = toPwm((float)maxPos);
    S.write(toPwm(30)); // 写入新角度值,控制舵机转动
    delay(500);         // 等待该舵机转到目标角度.
}
#endif