#ifndef MOTOR_H
#define MOTOR_H

#include<Arduino.h>

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

int speed(int speedValue){
    return speedValue;
};


class Moto{
  int pin1;
  int pin2;
  int pin3;
  int num;
  int state;
  public:
    Moto(int p1,int p2,int p3){
      pin1 = p1;
      pin2 = p2;
      pin3 = p3;
      num = 0;     // 0 -0,1
      state =0;    //0 stop, -1 backward, 1, forward
    // Set the motor control pins to outputs
      pinMode(p1, OUTPUT);
      pinMode(p2, OUTPUT);
      pinMode(p3, OUTPUT);
    }
    void backward(){
      num = 255;
      state = -1;
      act();
    }
    void forward(){
      num = 255;
      state = 1;
      act();
    }
    void stop(){
      num = 0;
      state = 0;
      act();
    }

    void act(){
      switch(state){
      case -1:
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, HIGH);
        analogWrite(pin3, speed(num));
        break;
      case 0:
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, LOW);
        break;
      case 1:
        Serial.println("forward"); 
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, LOW);
        Serial.println(num);
        Serial.println(state);
        analogWrite(pin3, speed(num));
        break;
      }
    }
};

class Rover{
  int lastState;
  Moto *left;
  Moto *right;
public:
  Rover(Moto* l, Moto* r) : left(l), right(r) {
    left = l;
    right = r;
  }
  static Rover* getInstance() {
    Moto* l = new Moto(MOTOR_LEFT_PIN_1,MOTOR_LEFT_PIN_2,MOTOR_ENB);
    Moto* r = new Moto(MOTOR_RIGTH_PIN_1,MOTOR_RIGTH_PIN_2,MOTOR_ENA);
    return new Rover(l,r);
  }
  void lauch(){
    left->forward();
    right->forward();
  }
  void act(){
    left->act();
    right->act();
  }
  void stop(){
    left->stop();
    right->stop();
  }
};


#endif
