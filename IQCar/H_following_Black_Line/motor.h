#ifndef __MOTOR_H__
#define __MOTOR_H__
#include <Arduino.h>
struct Motor {
  unsigned int pin1;  // 引脚 1
  unsigned int pin2;  // 引脚 2
  unsigned int speed;  // 引脚 enable
};


struct Rover {
  struct Motor rightWheel;
  struct Motor leftWheel;
};

void control_motor(struct Motor motor);  // 修正函数名

void move_forward(struct Rover car);  // 修正参数类型
void stop(struct Rover car);  // 修正参数类型
void move_backward(struct Rover car);  // 修正参数类型
void turn_right(struct Rover car);  // 修正参数类型
void turn_left(struct Rover car);  // 修正函数声明
#endif

enum Direction {
  Forward,  // 向前转
  Stay,     // 静止不动
  Backward  // 向后转
};



void Motor_roll(struct Motor motor, enum Direction value) {
  switch (value) {
    case Forward:  // 向前转
      digitalWrite(motor.pin1, HIGH);
      digitalWrite(motor.pin2, LOW);
      break;
    case Stay:  // 静止不动
      digitalWrite(motor.pin1, LOW);
      digitalWrite(motor.pin2, LOW);
      break;
    case Backward:  // 向后转
      digitalWrite(motor.pin1, LOW);
      digitalWrite(motor.pin2, HIGH);
      break;
  }
}

// 车子向前
void move_forward(struct Rover car) {
  Motor_roll(car.rightWheel, Forward);
  Motor_roll(car.leftWheel, Forward);
}
// 车子停止
void stop(struct Rover car) {
  Motor_roll(car.rightWheel, Stay);
  Motor_roll(car.leftWheel, Stay);
}
// 车子向后
void move_backward(struct Rover car) {
  Motor_roll(car.rightWheel, Backward);
  Motor_roll(car.leftWheel, Backward);
}
// 车子右转
void turn_right(struct Rover car) {
  Motor_roll(car.rightWheel, Stay);
  Motor_roll(car.leftWheel, Forward);
  delay(1);   // 右转一下(毫秒)后，马上直行
  Motor_roll(car.rightWheel, Forward);
}
// 车子左转
void turn_left(struct Rover car) {
  Motor_roll(car.rightWheel, Forward);
  Motor_roll(car.leftWheel, Stay);
  delay(1);   // 左转一下(毫秒)后，马上直行
  Motor_roll(car.leftWheel, Forward);
}

// 绑定马达引脚
void bind_motor(struct Motor motor) {
  pinMode(motor.pin1, OUTPUT);
  pinMode(motor.pin2, OUTPUT);
  pinMode(motor.speed, OUTPUT);
}