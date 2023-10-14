// 本程序使用 L298N驱动板上的 ENA 和 ENB 两个引脚，通过 PWM 信号来控制小车的行进速度。
// 在Arduino上使用servo库时，不能在引脚9,10上使用PWM。
//Arduino有3个定时器和6个PWM输出引脚。定时器和PWM输出之间的关系是：
//引脚5和6：由timer0控制
//引脚9和10：由timer1控制
//引脚11和3：由timer2控制

#include "motor.h"
#define S1  A0
#define S2  A1
#define S3  A2
#define S4  A3

struct Car myCar = Car_init();

void IRS(void){
  if(S1==LOW&S2==HIGH&S3==HIGH&S4==LOW){   // 正常
    switch(myCar.lastState){
      case 6:   // 0110
         keepMove(&myCar);
      break;
      case 3:  // 0011
      
      break;
    }
  }
}
void setup() {
  Serial.begin(9600);

}

void loop() {

}

