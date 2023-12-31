#ifndef __MYMOTO_H__
#define __MYMOTO_H__

#include <Arduino.h>
#include <stdio.h>
// 车上马达 A 的引脚连接
#define L_PIN_1 6
#define L_PIN_2 7
#define L_PWM 11

#define R_PIN_1 4
#define R_PIN_2 5
#define R_PWM 10

#define MAX_MOTO_SPEED 80

class Moto {
  unsigned char name;
  int pin1;
  int pin2;
  int pwm_pin;
  int speed = 0;

public:
  Moto(unsigned char n,int p1, int p2, int pwm) {
    name = n;
    pin1 = p1;
    pin2 = p2;
    pwm_pin = pwm;
    // Set the motor control pins to outputs
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
  }
  static Moto* getInst(unsigned char name){
    if(name =='L')
      return new Moto('L',L_PIN_1, L_PIN_2, L_PWM);
    if(name=='R')
      return new Moto('R',R_PIN_1, R_PIN_2, R_PWM);
    return NULL;
  }
  void stop() {
    lastspeed =speed;
    speed = 0;
    act();
  }
  void start(){
    forward();
  }
  void forward() {
    lastspeed = speed;
    speed = MAX_MOTO_SPEED;
    act();
  }
  void back(){
    lastspeed = speed;
    speed = - MAX_MOTO_SPEED;
    act();
  }
  void Moto_DebugInfo(){
    if(lastspeed==speed)
      return;
    lastspeed = speed;
    Serial.print("马达:");
    Serial.print(name);
    Serial.print("--");
    Serial.println(speed);
  }
  void keepRun(){
    act();
  }


private:
  int lastspeed=0;
  void act()
  {
    if (speed > 0)
    {
      digitalWrite(pin1, 1);
      digitalWrite(pin2, 0);
    }
    else if (speed < 0)
    {
      digitalWrite(pin1, 0);
      digitalWrite(pin2, 1);
    }
    else
    {
      digitalWrite(pin1, 0);
      digitalWrite(pin2, 0);
    }
    analogWrite(pwm_pin, abs(speed));
    delay(20);
  }
};
class Rover {
  int lastState,state = 0;
  Moto* lMotor=NULL;
  Moto* rMotor=NULL;
  String command="None";
  String lastCommand="None";
  int num = 0;
public:
  Rover(Moto* l, Moto* r)
    : lMotor(l), rMotor(r) {
    lMotor = l;
    rMotor = r;
  }
  static Rover* getInstance() {
    Moto* l = Moto::getInst('L');
    Moto* r = Moto::getInst('R');
    return new Rover(l, r);
  }

  void left() {
    if(!health_check()) return;
    command = "left";
    lMotor->back();
    rMotor->forward();
  }
  void right() {
    command = "right";
    lMotor->forward();
    rMotor->back();
  }
  void forward() {
    if(!health_check()) return;
    command = "forward";
    lMotor->forward();
    rMotor->forward();
  }
  void back() {
    if (!health_check())
      return;
    command = "back";
    lMotor->back();
    rMotor->back();
  }
  void keepMove(){
    lMotor->keepRun();
    rMotor->keepRun();
  }
  void stop() {
    if(!health_check()) return;
    command = "stop";
    lMotor->stop();
    rMotor->stop();
  }
  void act(uint8_t command)
  {
    Serial.print(" care command : ");
    Serial.println((char)command);
    switch (command)
    {
    case '2':
      forward();
      break;
    case '4':
      left();
      break;
    case '5':
      stop();
    case '6':
      right();
    case '8':
      back();
    case 'm':
      keepMove();
      break;
    default:
      keepMove();
      break;
    }
  }
  void Rover_DebugInfo(){
    if(!health_check()) return;
    if(command!=lastCommand){
      lastCommand = command;
      Serial.println("command = "+ command);
    }
    lMotor->Moto_DebugInfo();
    rMotor->Moto_DebugInfo();
  }
private:
  bool health_check(){
    bool ret = lMotor!=NULL && rMotor!=NULL;
    if(!ret){
      Serial.println("马达没有初始化。");
    }
    return ret;
  }
};

#endif
