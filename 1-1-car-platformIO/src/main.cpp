#include <Arduino.h>
#include "myMoto.h"
#include "myServo.h"
#include "traceSensor.h"
#include "irControl.h"
#include "ultraSound.h"
#include "InfraRED.h"

Rover *rover = NULL;

void OBS_Enable()
{
  uint8_t signal = OBS_Detect();
  switch (signal)
  {
  case 3: // 0b11
    rover->stop();
    break;
  case 0: // 0b00
    rover->forward();
    break;
  case 2: // 0b10
    rover->right();
    break;
  case 1:
    rover->left();
    break;
  default:
    rover->keepMove();
    break;
  }
}

void Trace_Enable()
{
  uint8_t signal = trace_check();
  switch (signal)
  {
  case 3:  // 0b11
    rover->stop();
    break;
  case 0:  // 0b00
    rover->forward();
    break;
  case 2:  // 0b10
    rover->right();
    break;
  case 1:
    rover->left();
    break;
  default:
    rover->keepMove();
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
