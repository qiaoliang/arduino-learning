#include <Arduino.h>
#include "myMoto.h"
#include "myServo.h"
#include "traceSensor.h"
#include "irControl.h"
#include "ultraSound.h"
#include "InfraRED.h"

Rover *rover = NULL;


void Trace_Enable()
{
  unsigned char signal = trace_check();
  switch (signal)
  {
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
/**
 * @brief 根据遥控器的按键移动小车. 2 - 向前, 4 - 向左 , 5 - 停 , 6 - 向右 , 8 - 向后
 *
 */
void IRControl_Enable()
{
  if (rover == NULL)
  {
    Serial.println("rover 需要先初始化。");
    return;
  }
  uint8_t signal = IR_detect();
  rover->act(signal);
}

void setup()
{
  Serial.begin(9600);
  UltraSound_Init();
  IR_Init();
  rover = Rover::getInstance();
}

void loop()
{
  rover->Rover_DebugInfo();
  UltraSound_Following_enable();
  IRControl_Enable();
}
