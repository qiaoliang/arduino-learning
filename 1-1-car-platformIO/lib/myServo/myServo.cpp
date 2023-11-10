#ifndef __MYSERVO_H__
#define __MYSERVO_H__

#include <Arduino.h>
#include <Servo.h>

#define SERVO_PIN A0
#define INIT_ANGLE 90
Servo myservo;

int preAngle = INIT_ANGLE;
/**
 * @brief 超声波舵机的初始化
 *
 */
void MyServo_Init() {
  myservo.attach(SERVO_PIN);
  myservo.write(INIT_ANGLE);
}

/**
 * @brief
 *
 */

void MyServo_rotate(int offset) {
  if(!myservo.attached())
    return;
  int targetAngle = offset + preAngle;
  if( targetAngle>=180 ) {
    targetAngle =180;
  }else if(targetAngle <=0){
    targetAngle =0;
  }

  myservo.write(targetAngle);
  delay(1000);
  preAngle = targetAngle;
}

// 旋转到某一绝对角度。degree 为目标角度。
void MyServo_Set(int degree) {
    if (degree < 0) {
      degree = 0;
    }
    if (degree > 180) {
      degree = 180;
    }
    myservo.write(degree);
    delay(1000);
}

#endif
