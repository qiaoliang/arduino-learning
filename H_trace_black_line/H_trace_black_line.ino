#include "motor.h"
#include "myservo.h"
#include "tracesensor.h"
#include "irControl.h"
#include "ultrasound.h"
#include "InfraRED.h"


Rover* rover = NULL ;
int count =0;


void Trace_Enable(){
  unsigned char signal = trace_check();
  switch(signal){
    case 0b0000:
    rover->stop();
    rover->Rover_DebugInfo();
    break;
    case 0b0110:
    rover->forward();
    rover->Rover_DebugInfo();
    break;
    case 0b1100:
    case 0b1110:
    rover->right();
    rover->Rover_DebugInfo();
    break;
    case 0b0011:
    case 0b0111:
    rover->left();
    rover->Rover_DebugInfo();
    break;
    case 0b1111:
    rover->stop();
    rover->Rover_DebugInfo();
    break;
  }
}

void IRControl_Enable(){
  if(rover == NULL){
    Serial.println("rover 需要先初始化。");
  }
  uint8_t signal = IR_detect();
  switch(signal){
  case KEY_2:
    rover->start();
    rover->Rover_DebugInfo();
    break;
  case KEY_4:
    rover->left();
    rover->Rover_DebugInfo();    
    break;
  case KEY_6:
    rover->right();
    rover->Rover_DebugInfo();
    break;
  case KEY_5:
    rover->stop();
    rover->Rover_DebugInfo();
    break;
  case KEY_8:
    break;
  default:
    break;
  }
}

void setup() {
  Serial.begin(9600);
  rover = Rover::getInstance();
  rover->start();
  TraceSensor_Init();
  UltraSound_Init();
  OBS_Init();
  MyServo_Init();



  //IR_Init();
}

int degree =10;
void loop() {
  Trace_DebugInfo();
  US_DebugInfo();
  OBS_DebugInfo();
  degree = -degree;
  MyServo_rotate(degree);
  rover->Rover_DebugInfo();
  //Trace_Enable();
  //IRControl_Enable();
  //rover->Rover_DebugInfo();

}
