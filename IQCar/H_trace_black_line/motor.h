#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include <stdio.h>
// 车上马达 A 的引脚连接
#define MOTOR_RIGTH_PIN_1 6
#define MOTOR_RIGTH_PIN_2 7
#define MOTOR_ENA 11

#define MOTOR_LEFT_PIN_1 4
#define MOTOR_LEFT_PIN_2 5
#define MOTOR_ENB 10

#define SPEEDUNIT 50
#define MAX_SPEED 3

static uint8_t POSITIONS[3][2] = { { LOW, LOW }, { HIGH, LOW }, { LOW, HIGH } };
static uint8_t SPEEDS[4] = { 0, 100, 175, 250 };
class Moto {
  unsigned char name; 
  int pin1;
  int pin2;
  int pin3;
  int curPos = 0;
  int curGear = 0;
public:
  Moto(int p1, int p2, int p3) {
    pin1 = p1;
    pin2 = p2;
    pin3 = p3;
    // Set the motor control pins to outputs
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
  }
  void setName(unsigned char value){
    name = value;
  }
  void speedDown() {
    curGear = curGear > 0 ? curGear - 1 : curGear;
    act();
  }
  void speedUp() {
    curGear = curGear < 3 ? curGear + 1 : curGear;
    act();
  }
  void stop() {
    curPos = 0;
    curGear = 0;
    act();
  }
  void backward() {
    curPos = 2;
    curGear = curGear != 0 ? curGear : 1;
    act();
  }
  void forward() {
    curPos = 1;
    curGear = curGear != 0 ? curGear : 1;
    act();
  }
  
 char* currentState() {
    char* result = (char*)malloc(10 * sizeof(char));;
    sprintf(result, "%c - %d : %d\n", name,curPos,curGear);
    return result;
  }
private:
  void act() {
    digitalWrite(pin1, POSITIONS[curPos][0]);
    digitalWrite(pin2, POSITIONS[curPos][1]);
    analogWrite(pin3, SPEEDS[curGear]);
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
  void powerUp() {
    lMotor->speedUp();
    rMotor->speedUp();
  }
  void powerDown() {
    lMotor->speedDown();
    rMotor->speedDown();
  }
};

#endif
