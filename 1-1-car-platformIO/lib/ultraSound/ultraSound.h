#ifndef __ULTRASOUND_H__
#define __ULTRASOUND_H__

#define ULTRASOUND_OUT_PIN  2
#define ULTRASOUND_IN_PIN   3
#include <Arduino.h>

float UltSound_detect();
void UltraSound_Init();
void US_DebugInfo();
#endif