#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

// 车上马达 A 的引脚连接
#define MOTOR_RIGTH_PIN_1 4
#define MOTOR_RIGTH_PIN_2 5
#define MOTOR_ENA 6

#define MOTOR_LEFT_PIN_1 7
#define MOTOR_LEFT_PIN_2 8
#define MOTOR_ENB 9
#define SPEEDUNIT 50
#define MAX_SPEED 3

enum Status {
  FORWARD,
  STOP,
  BACKWARD
};

int speed(int speedValue) {
  if (speedValue == 0) return 0;
  return 100 + 75 * (speedValue-1);
};

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
    pinMode(p3, OUTPUT);
  }
  void powerDown() {
    if (num > 0) {
      lastnum = num;
    }
  }
  void powerUp() {
    if (num < 3) {
      lastnum = num;
      num+=1;
    }
  }
  void powerOff(){
    lastnum = num;
    num = 0;
  }
  void backward() {
    laststate = state;
    state = -1;
  }
  void forward() {
    laststate = state;
    state = 1;
  }
  void setSpeed(int number) {
    lastnum = num;
    num = number;
  }
  void stop() {
    laststate = state;
    state = 0;
    lastnum = num;
    num = 0;
  }
  void act() {
    switch (state) {
      case -1:
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, HIGH);
        break;
      case 0:
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, LOW);
        break;
      case 1:
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, LOW);
        break;
    }
    analogWrite(pin3, speed(num));
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
  void powerUp() {
    syncSpeed();
    lMotor->powerUp();
    rMotor->powerUp();
  }
  void powerDown() {
    syncSpeed();
    lMotor->powerDown();
    rMotor->powerDown();
  }
  void syncSpeed() {
    int lnum = lMotor->getNum();
    int rnum = rMotor->getNum();
    if(lnum!=rnum){
      int speed = lnum > rnum ? lnum : rnum;
      lMotor->setSpeed(speed);
      rMotor->setSpeed(speed);
    }
  }
  void left() {
    lMotor->powerOff();
    rMotor->powerUp();
  }
  void right() {
    lMotor->powerUp();
    rMotor->powerOff();
  }
  void forward() {
    lMotor->forward();
    rMotor->forward();
  }
  void backward() {
    lMotor->backward();
    rMotor->backward();
  }

  void act() {
    lMotor->act();
    rMotor->act();
  }
  void stop() {
    lMotor->stop();
    rMotor->stop();
  }
  void printState(){
    Serial.print("Left: ");
    pState(lMotor);
    Serial.print("Right: ");
    pState(rMotor);    
  }
  void pState(Moto* moto){
    Serial.print(moto->getState());
    Serial.print(":");
    Serial.println(moto->getNum());
  }
};


#endif
