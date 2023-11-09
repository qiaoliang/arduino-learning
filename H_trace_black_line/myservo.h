#ifndef MYSERVO_H
#define MYSERVO_H
#include <Servo.h>

#define SERVO_PIN A0
#define INIT_ANGLE 90
Servo myservo;

int preAngle = INIT_ANGLE;

void MyServo_Init() {
  myservo.attach(SERVO_PIN);
  myservo.write(INIT_ANGLE);
}


//以当前角度为基点，偏转一定的角度。正数为顺时针，负数为逆时针。
void MyServo_rotate(int offset) {
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
