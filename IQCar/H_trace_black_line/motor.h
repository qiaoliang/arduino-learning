#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

// 车上马达 A 的引脚连接
#define MOTOR_RIGTH_PIN_1 6
#define MOTOR_RIGTH_PIN_2 7
#define MOTOR_ENA 11

#define MOTOR_LEFT_PIN_1 4
#define MOTOR_LEFT_PIN_2 5
#define MOTOR_ENB 10

#define SPEEDUNIT 50
#define MAX_SPEED 3
class Moto {
  int pin1;
  int pin2;
  int pin3;
  int lastnum, num = 0;      // 0,1(100),2(175),3(250)
  int laststate, state = 0;  //0 stop, -1 backward, 1, forward
public:
  Moto(int p1, int p2, int p3) {
    pin1 = p1;
    pin2 = p2;
    pin3 = p3;
    // Set the motor control pins to outputs
    pinMode(p1, OUTPUT);
    pinMode(p2, OUTPUT);
  }
  void powerDown() {
    if (num > 0) {
      lastnum = num;
    }
  }
  void powerUp() {
      num += 1;
  }
  void stop() {
  digitalWrite(pin1,LOW);
  digitalWrite(pin2,LOW);
  analogWrite(pin3,0);
  }
  void backward() {
  digitalWrite(pin1,LOW);
  digitalWrite(pin2,HIGH);
  analogWrite(pin3,200);
  }
  void forward() {
  digitalWrite(pin1,HIGH);
  digitalWrite(pin2,LOW);
  analogWrite(pin3,200);
  }
  void speed(int number) {
    num = 200;
  }
  void start() {
    laststate = state;
    state = 1;
    lastnum = num;
    num = 1;
  }
  void act() {
    switch (state) {
      case 0:
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, LOW);
        analogWrite(pin3, 0);
        break;
      case 1:
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, LOW);
        analogWrite(pin3, 200);
        break;
    }
  }
  int getNum() {
    return num;
  }
  int getLastNum() {
    return lastnum;
  }
  int getState() {
    return state;
  }
  int getLastState() {
    return laststate;
  }
};
class Rover {
  int lastState;
  Moto* lMotor;
  Moto* rMotor;
  int num = 0;
  int state = 0;
public:
  Rover(Moto* l, Moto* r)
    : lMotor(l), rMotor(r) {
    lMotor = l;
    rMotor = r;
  }
  static Rover* getInstance() {
    Moto* l = new Moto(MOTOR_LEFT_PIN_1, MOTOR_LEFT_PIN_2, MOTOR_ENB);
    Moto* r = new Moto(MOTOR_RIGTH_PIN_1, MOTOR_RIGTH_PIN_2, MOTOR_ENA);
    return new Rover(l, r);
  }

  void left() {
    lMotor->stop();
    rMotor->forward();
  }
  void right() {
    lMotor->forward();
    rMotor->stop();
  }
  void forward() {
    lMotor->forward();
    rMotor->forward();
  }
  void stop() {
    lMotor->stop();
    rMotor->stop();
  }
  void printState() {
    Serial.print("Left: ");
    pState(lMotor);
    Serial.print("Right: ");
    pState(rMotor);
  }
  void pState(Moto* moto) {
    Serial.print(moto->getState());
    Serial.print(":");
    Serial.println(moto->getNum());
  }
};

#endif
