#ifndef __INFRARED_H__
#define __INFRARED_H__
// 车上左右红外测距接收器的引脚
#define INFRA_DISTANCE_DETECT_LEFT_PIN  8
#define INFRA_DISTANCE_DETECT_RIGHT_PIN 9

void OBS_Init();
unsigned char OBS_Detect();
void OBS_DebugInfo();
#endif