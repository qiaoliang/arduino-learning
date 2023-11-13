#ifndef __ULTRASOUND_H__
#define __ULTRASOUND_H__

#define ULTRASOUND_TRIG_PIN  2
#define ULTRASOUND_ECHO_PIN   3
#include <Arduino.h>

void UltraSound_Init();
void UltraSound_Following_enable();
void US_DebugInfo();
#endif