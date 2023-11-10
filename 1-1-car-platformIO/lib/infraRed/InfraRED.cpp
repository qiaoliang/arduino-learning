#include <Arduino.h>
#include "InfraRED.h"

void OBS_Init() {
  // 绑定红外测距接收器
  pinMode(INFRA_DISTANCE_DETECT_RIGHT_PIN, INPUT);
  pinMode(INFRA_DISTANCE_DETECT_LEFT_PIN, INPUT);
}
uint8_t last_obsdata,current_obsdata;
uint8_t OBS_Detect()
{
  last_obsdata = current_obsdata;
  uint8_t left = digitalRead(INFRA_DISTANCE_DETECT_LEFT_PIN);
  uint8_t right = digitalRead(INFRA_DISTANCE_DETECT_RIGHT_PIN);
  current_obsdata = (left<<1)|right;
  return current_obsdata;
}

void OBS_DebugInfo(){
  OBS_Detect();
  if(current_obsdata!= last_obsdata){
    Serial.print("OBS data: ");
    Serial.println(current_obsdata);
  }
}