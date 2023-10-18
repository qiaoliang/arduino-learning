// 本程序使用 L298N驱动板上的 ENA 和 ENB 两个引脚，通过 PWM 信号来控制小车的行进速度。
// 在Arduino上使用servo库时，不能在引脚9,10上使用PWM。
//Arduino有3个定时器和6个PWM输出引脚。定时器和PWM输出之间的关系是：
//引脚5和6：由timer0控制
//引脚9和10：由timer1控制
//引脚11和3：由timer2控制
#include "timer1.h"
#include "motor.h"
#include "ircontrol.h"
#include "tracesensor.h"


#define OBS_PIN1  12      // 右红外避障的引脚
#define OBS_PIN2  0       // 左红外避障的引脚

ISR(PCINT1_vect){    // 对A4 引脚 中断的处理函数
    IrReciver_exec();
}


Rover* rover = Rover::getInstance();

void setup() {
  Serial.begin(9600);
  Timer1_init();
  IrReceiver_ISR_init();
  rover->lauch();
}

void loop() {
  rover->act();
}

