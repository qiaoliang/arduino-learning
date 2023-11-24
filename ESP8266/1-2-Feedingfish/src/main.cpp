/**
 * @file main.cpp
 * @author qiaoliang
 * @brief 喂鱼器.  每隔1小时,投喂一次,每次三转.
 * @version 0.1
 * @date 2023-11-24
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <Arduino.h>

#include "MyServo.h"
#include "feeding.h"

void setup()
{
  Serial.begin(115200);           // 开启串口通信 波特率115200,串口监视器也要相同波特率,不然会乱码
  pinMode(LED_BUILTIN, OUTPUT);   // 初始化8266开发板LED信号灯的GPIO口为输出.
  digitalWrite(LED_BUILTIN, LOW); // Mini ESP8266板LED_BUILTIN=GPIO 2,LOW=亮灯,HIGH=灭灯
  Servo_Init();
  Feeding_Init();
  digitalWrite(LED_BUILTIN, LOW); // 亮灯
}

void loop()
{
  keepRunning();
  //---------------只监听电脑串口输入指令-------------------
  if (Serial.available() > 0)
  {                                 // 有串口数据>0字节
    String t = Serial.readString(); //?SDCHRA XYZBTE
    t.trim();                       // 删首尾空格与换行
    Serial.println(t);              // 串口回显指令
    go(t.toFloat());
  }
}
