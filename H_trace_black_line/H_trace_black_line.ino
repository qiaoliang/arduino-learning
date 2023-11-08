// 本程序使用 L298N驱动板上的 ENA 和 ENB 两个引脚，通过 PWM 信号来控制小车的行进速度。

/**
 * 当前版本 V2.0
 * 1. 只使用车子前端的四个红外传感器，还是无法遥控小车。
 * 2. 需要仔细调节四个红外传感器，让两个中间的挨近些，两边传感器与中间的传感器间隔大一些。
 * 3. 没有使用遥控的外部中断和定时器中断。
 */
#include "motor.h"
#include "tracesensor.h"

Rover* rover = Rover::getInstance();
int count =0;

void setup() {
  Serial.begin(9600);
  traceSensor_Init();
}

unsigned char oldtrace;
void loop() {
  unsigned char trace = trace_check();
  count++;
  if(trace !=oldtrace){
    Serial.println(trace);
    oldtrace = trace;
  }
}
