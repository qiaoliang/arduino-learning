// 本程序使用 L298N驱动板上的 ENA 和 ENB 两个引脚，通过 PWM 信号来控制小车的行进速度。
// 在Arduino上使用servo库时，不能在引脚9,10上使用PWM。
//Arduino有3个定时器和6个PWM输出引脚。定时器和PWM输出之间的关系是：
//引脚5和6：由timer0控制
//引脚9和10：由timer1控制
//引脚11和3：由timer2控制
#include "timer1.h"
#include "motor.h"
#include "ircontrol.h"

#define TRACE_PIN1  A0  // 寻迹引脚右1
#define TRACE_PIN2  A1  // 寻迹引脚右2
#define TRACE_PIN3  A2  // 寻迹引脚右3
#define TRACE_PIN4  A3  // 寻迹引脚右4


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
void trace_loop(){
  unsigned char trace_signal = digitalRead(TRACE_PIN4);
  trace_signal = (trace_signal<<1)|digitalRead(TRACE_PIN3);
  trace_signal = (trace_signal<<1)|digitalRead(TRACE_PIN2);
  trace_signal = (trace_signal<<1)|digitalRead(TRACE_PIN1);
    Serial.println(trace_signal);
  if (trace_signal == 0B0000) {
    //stop(car);
  }
  if (trace_signal == 0b1100) {
    //turn_right(car);
  }
  if (trace_signal == 0b0011) {
    //turn_left(car);
  }
  if (trace_signal == 0b0110) {
    //move_forward(car);
  }
}
void loop() {
  rover->act();
}

