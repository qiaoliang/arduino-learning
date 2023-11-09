#include "motor.h"
#include "tracesensor.h"
#include "irControl.h"
#include "ultrasound.h"

Rover* rover = Rover::getInstance();
Moto* moto = Moto::getInst('L');
int count =0;

void setup() {
  Serial.begin(9600);
  TraceSensor_Init();
  IR_Init();
  UltraSound_Init();
  moto->Moto_DebugInfo();
}

float lastvalue=0.0;

void loop() {
  US_DebugInfo();
  IR_DebugInfo();
  Trace_DebugInfo();
  moto->Moto_DebugInfo();
  long signal = IR_detect();
  switch(signal){
  case 2:
    break;
  case 4:
    break;
  case 6:
    break;
  case 5:
    break;
  case 8:
    break;
  default:
    break;
  }

  signal = trace_check();
  switch(signal){
    case 0b0000:
    rover->stop();
    break;
    case 0b0110:
    rover->forward();
    break;
    case 0b1100:
    case 0b1110:
    rover->right();
    break;
    case 0b0011:
    case 0b0111:
    rover->left();
    break;
    case 0b1111:
    rover->stop();
    break;
  }
}
