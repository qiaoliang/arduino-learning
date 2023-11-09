#include "motor.h"
#include "tracesensor.h"
#include "irControl.h"
#include "ultrasound.h"

Rover* rover ;
Moto* moto = Moto::getInst('L');
int count =0;

void setup() {
  Serial.begin(9600);
  TraceSensor_Init();
  IR_Init();
  UltraSound_Init();
  //rover = Rover::getInstance();
  //rover->start();
}

void enable_trace(){
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

void enable_irControl(){
  long signal = IR_detect();
  switch(signal){
  case 2:
    rover->start();
    rover->Rover_DebugInfo();
    break;
  case 4:
    rover->left();
    rover->Rover_DebugInfo();    
    break;
  case 6:
    rover->right();
    rover->Rover_DebugInfo();
    break;
  case 5:
    rover->stop();
    rover->Rover_DebugInfo();
    break;
  case 8:
    break;
  default:
    break;
  }
}
void loop() {
  IRTEST();
  US_DebugInfo();
  //IR_DebugInfo();
  Trace_DebugInfo();

  //enable_trace();
  //enable_irControl();

}
