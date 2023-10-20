// 本程序使用 L298N驱动板上的 ENA 和 ENB 两个引脚，通过 PWM 信号来控制小车的行进速度。

/**
 * 当前版本 V2.0
 * 1. 只使用车子前端的四个红外传感器，还是无法遥控小车。
 * 2. 需要仔细调节四个红外传感器，让两个中间的挨近些，两边传感器与中间的传感器间隔大一些。
 * 3. 没有使用遥控的外部中断和定时器中断。
 */
#include "timer1.h"
#include "motor.h"
#include "ircontrol.h"
#include "tracesensor.h"


#define OBS_PIN1  12      // 右红外避障的引脚
#define OBS_PIN2  0       // 左红外避障的引脚

ISR(PCINT1_vect){    // 对A4 引脚 中断的处理函数
    IrReciver_Routine();
}

Rover* rover = Rover::getInstance();

void setup() {
  Serial.begin(9600);
  traceSensor_Init();
//  Timer1_init();
//  IrReceiver_ISR_init();
  rover->forward();
}

void loop() {
  trace_check();
}
