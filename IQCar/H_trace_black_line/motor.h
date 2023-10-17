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


struct Motor {
    unsigned int pin1;  // 电源正引脚 1
    unsigned int pin2;  // 电源负引脚 2
    unsigned int pin3;  // 引脚 enable，PWM 信号
    int direction;
    int num;
};

void Motor_stop(struct Motor *m) {
    m->num = 0;
    digitalWrite(m->pin1, LOW);
    digitalWrite(m->pin2, LOW);
    analogWrite(m->pin3, speed(m->num));
};

void Motor_slowdown(struct Motor *m) {
    m->num = m->num>=1?m->num-1:0;
    analogWrite(m->pin3, speed(m->num));
};

void Motor_speedup(struct Motor *m) {
    m->num = m->num<MAX_SPEED?m->num+1:MAX_SPEED;
    analogWrite(m->pin3, speed(m->num));
};
void Motor_keepmoving(struct Motor *m) {
    analogWrite(m->pin3, speed(m->num));
};
void Motor_forward(struct Motor *m) {
    digitalWrite(m->pin1, LOW);
    digitalWrite(m->pin2, HIGH);
    analogWrite(m->pin3, speed(m->num));
};
void Motor_backward(struct Motor *m) {
    digitalWrite(m->pin1, HIGH);
    digitalWrite(m->pin2, LOW);
    analogWrite(m->pin3, speed(m->num));
};

void Motor_init(struct Motor *m, int p1, int p2, int p3) {
    m->pin1 = p1;
    m->pin2 = p2;
    m->pin3 = p3;
    m->num = 0;
    m->direction = STOP;
    // Set the motor control pins to outputs
    pinMode(p1, OUTPUT);
    pinMode(p2, OUTPUT);
    pinMode(p3, OUTPUT);
};

void initializeMotors() {
}


struct Car{
  struct Motor left;
  struct Motor right;
  int lastState;
};
keepMove(struct Car *myCar){
  Motor_speedup(&(myCar->left));
  Motor_speedup(&(myCar->right));
}


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
      num = 0;
      state =0;
    // Set the motor control pins to outputs
      pinMode(p1, OUTPUT);
      pinMode(p2, OUTPUT);
      pinMode(p3, OUTPUT);
    }
    void run(){
       analogWrite(pin3, speed(num));
    }
    void move(){
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
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, LOW);
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
  static getInstance(){
  }
  Rover(Moto *leftM,Moto *rightM){
    left = leftM;
    right = rightM;
  }
  void move(){
    left->run();
    right->run();
  }
  void turnRight(){

  }
  void trunLeft(){

  }
};
Moto left = Moto(MOTOR_LEFT_PIN_1,MOTOR_LEFT_PIN_2,MOTOR_ENB);
Moto right = Moto(MOTOR_LEFT_PIN_1,MOTOR_LEFT_PIN_2,MOTOR_ENB);
Rover rover = Rover(&left,&right);

struct Car Car_init(){
  struct Motor rightW;
  struct Motor leftW;
  Motor_init(&leftW, MOTOR_LEFT_PIN_1, MOTOR_LEFT_PIN_2, MOTOR_ENB);
  Motor_init(&rightW, MOTOR_RIGTH_PIN_1, MOTOR_RIGTH_PIN_2, MOTOR_ENA);
  struct Car mycar;
  mycar.left = leftW;
  mycar.right = rightW;
  mycar.lastState = 6; //0110
  return mycar;
}


#endif
