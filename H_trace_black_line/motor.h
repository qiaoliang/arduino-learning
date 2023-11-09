#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include <stdio.h>
// 车上马达 A 的引脚连接
#define R_PIN_1 6
#define R_PIN_2 7
#define R_PWM 11

#define L_PIN_1 4
#define L_PIN_2 5
#define L_PWM 10

#define MAX_SPEED 10000
#define MIN_SPEED 1000
#define PWD_UNIT 255/MAX_SPEED

#define MAX_SPEED 3

class Moto {
  unsigned char name; 
  int pin1;
  int pin2;
  int pwm_pin;
  int speed;

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
    return new Moto('L',L_PIN_1, L_PIN_2, L_PWM);    
  }
  void setSpeed(int v){
    lastspeed = speed;
    speed= v;
  }

  void stop() {
    lastspeed =speed;
    speed =0;
  }
  void start(){
    lastspeed =speed;
    speed = MIN_SPEED;
  }
  void backward() {
    if(speed >0){
      lastspeed =speed;
      speed = -speed;
    }
    if(speed=0){
      lastspeed =speed;
      speed = -MIN_SPEED;
    }
    act();
  }
  void forward() {
    if(speed <0){
      lastspeed =speed;
      speed = -speed;
    }
    if(speed=0){
      lastspeed =speed;
      speed = MIN_SPEED;
    }
    act();
  }
  void Moto_DebugInfo(){
    if(lastspeed==speed)
      return;
    lastspeed = speed;
    Serial.print("马达:");
    Serial.print(String(name));
    Serial.print("--");
    Serial.println(speed);
  }
private:
  int lastspeed=-65535;
  void act() {
    if(speed>0 )
    {
      digitalWrite(pin1, 1);
      digitalWrite(pin2, 0);
    }else if(speed <0){
      digitalWrite(pin1, 0);
      digitalWrite(pin2, 1); 
    }else{
      digitalWrite(pin1, 0);
      digitalWrite(pin2, 0);
    }
    analogWrite(pwm_pin, speed*PWD_UNIT);
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
    Moto* r = new Moto("L",L_PIN_1, L_PIN_2, L_PWM);
    Moto* l = new Moto("R",R_PIN_1, R_PIN_2, R_PWM);
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
};

#endif
