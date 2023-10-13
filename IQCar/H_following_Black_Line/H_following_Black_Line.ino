//#include <TimerInterrupt.h>
#include <MsTimer2.h>
#include "motor.h"

// 车上马达的引脚
#define MOTOR_RIGTH_PIN_1 4
#define MOTOR_RIGTH_PIN_2 5
#define MOTOR_RIGTH_SPEED 6

#define MOTOR_LEFT_PIN_1  7
#define MOTOR_LEFT_PIN_2  8
#define MOTOR_LEFT_SPEED 9

#define USE_TIMER_1     true
// 车上寻迹模块的引脚
#define TRACE_PIN_1 A4  // 小车行驶方向的左侧第一个传感器
#define TRACE_PIN_2 A3
#define TRACE_PIN_3 A2
#define TRACE_PIN_4 A1


// 初始化车上的两个马达
struct Motor RMotor = { MOTOR_RIGTH_PIN_1, MOTOR_RIGTH_PIN_2, MOTOR_RIGTH_SPEED};
struct Motor LMotor = { MOTOR_LEFT_PIN_1, MOTOR_LEFT_PIN_2 ,MOTOR_LEFT_SPEED} ;

// 初始化小车
struct Rover car = { &RMotor, &LMotor };

unsigned int trace1,trace2,trace3,trace4;


void TimerHandler()
{
  // 读取寻迹引脚的反馈
  trace1= digitalRead(TRACE_PIN_1);
  trace2= digitalRead(TRACE_PIN_2);
  trace3= digitalRead(TRACE_PIN_3);
  trace4= digitalRead(TRACE_PIN_4);
}

#define TIMER_INTERVAL_MS        100L

void setup() {
  noInterrupts();//stop interrupts
  Serial.begin(9600);
  bind_motor(RMotor);
  bind_motor(LMotor);

  // 绑定寻迹感应模块
  pinMode(TRACE_PIN_1, INPUT);  // 寻迹右感应引脚
  pinMode(TRACE_PIN_2, INPUT);  // 寻迹左感应引脚
  pinMode(TRACE_PIN_3, INPUT);  // 寻迹右感应引脚
  pinMode(TRACE_PIN_4, INPUT);  // 寻迹左感应引脚
  MsTimer2::set(500, TimerHandler); // 500ms period
  MsTimer2::start();
}


void loop() {
  if (trace1 == LOW && trace2 == HIGH && trace3 == HIGH && trace4 == LOW ) {
    move_forward(car);
  }
  if (trace1 == HIGH && trace2 == HIGH && trace3 == LOW && trace4 == LOW ) {
    turn_right(car);
  }
  if (trace1 == LOW && trace2 == LOW && trace3 == HIGH && trace4 == HIGH ) {
    turn_left(car);
  }  
  if (trace1 == LOW && trace2 == LOW && trace3 == LOW && trace4 == LOW ) {
   stop(car);
  }   
}
