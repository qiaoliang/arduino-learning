// 本程序使用 L298N驱动板上的 ENA 和 ENB 两个引脚，通过 PWM 信号来控制小车的行进速度。
// 在Arduino上使用servo库时，不能在引脚9,10上使用PWM。
//Arduino有3个定时器和6个PWM输出引脚。定时器和PWM输出之间的关系是：
//引脚5和6：由timer0控制
//引脚9和10：由timer1控制
//引脚11和3：由timer2控制

#include "motor.h"

#define TRACE_PIN1  A0  // 寻迹引脚右1
#define TRACE_PIN2  A1  // 寻迹引脚右2
#define TRACE_PIN3  A2  // 寻迹引脚右3
#define TRACE_PIN4  A3  // 寻迹引脚右4

// 车上红外避障的引脚
#define OBS_PIN1  12
#define OBS_PIN2  0

struct Car myCar = Car_init();

void IRS(void){
  trace_loop();
}
void setup() {
  Serial.begin(9600);
  rover.init();
  rover.keepMove();
}
void trace_loop(){
  unsigned char trace_signal = digitalRead(TRACE_PIN4);
  trace_signal = (trace_signal<<1)|digitalRead(TRACE_PIN3);
  trace_signal = (trace_signal<<1)|digitalRead(TRACE_PIN2);
  trace_signal = (trace_signal<<1)|digitalRead(TRACE_PIN1);

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

}

