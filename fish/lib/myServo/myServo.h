#ifndef MYSERVO_H
#define MYSERVO_H
#include <Servo.h>

#define SERVO_PIN D1
void MyServo_Init();
void MyServo_rotate(int offset);
void MyServo_Set(int degree);

#endif
