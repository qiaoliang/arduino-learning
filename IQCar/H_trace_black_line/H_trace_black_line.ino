// 本程序使用 L298N驱动板上的 ENA 和 ENB 两个引脚，通过 PWM 信号来控制小车的行进速度。
// 在Arduino上使用servo库时，不能在引脚9,10上使用PWM。
//Arduino有3个定时器和6个PWM输出引脚。定时器和PWM输出之间的关系是：
//引脚5和6：由timer0控制
//引脚9和10：由timer1控制
//引脚11和3：由timer2控制
#include <IRremote.hpp>
#include <TimerOne.h>

#include "motor.h"

#define TRACE_PIN1  A0  // 寻迹引脚右1
#define TRACE_PIN2  A1  // 寻迹引脚右2
#define TRACE_PIN3  A2  // 寻迹引脚右3
#define TRACE_PIN4  A3  // 寻迹引脚右4

#define IR_RECEIVE_PIN A4  // 车上红外摇控接收器的引脚

#define KEY_2 0xE718FF00   // 手中红外遥控键盘的键值
#define KEY_5 0xE31CFF00
#define KEY_8 0xAD52FF00
#define KEY_4 0xF708FF00
#define KEY_6 0xA55AFF00


#define OBS_PIN1  12      // 右红外避障的引脚
#define OBS_PIN2  0       // 左红外避障的引脚

struct Car myCar = Car_init();

void timerIRS()
{
    Serial.println("interrupt");
}

void Timer_init(){
  Timer1.initialize(10000); //10ms
  Timer1.attachInterrupt(timerIRS);
}

void setup() {
  Serial.begin(9600);
  Timer_init();
  IrReceiver.begin(IR_RECEIVE_PIN); // 绑定遥控器接收模块
  rover.init();
  rover.keepMove();
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

}

