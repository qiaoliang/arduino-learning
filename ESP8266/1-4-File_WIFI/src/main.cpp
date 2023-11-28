/**
 * @file main.cpp
 * @author qiaoliang
 * @brief 喂鱼器.  每隔1小时,投喂一次,每次三转.
 * @version 0.1
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <Arduino.h>
#include <LittleFS.h> // ESP8266开发板自带4MB闪存空间,可以用来读写存删文件
#include "MyServo.h"
#include "feeding.h"
#include "FeedConfig.h"

FeedConfig sysConfig;
void setup()
{
  Serial.begin(115200);           // 开启串口通信 波特率115200,串口监视器也要相同波特率,不然会乱码
  pinMode(LED_BUILTIN, OUTPUT);   // 初始化8266开发板LED信号灯的GPIO口为输出.
  digitalWrite(LED_BUILTIN, LOW); // Mini ESP8266板LED_BUILTIN=GPIO 2,LOW=亮灯,HIGH=灭灯
  Servo_Init();
  Feeding_Init();
  sysConfig.Init();
}

void loop()
{
  keepRunning();
  //---------------只监听电脑串口输入指令-------------------
  if (Serial.available() > 0)
  {                                 // 有串口数据>0字节
    String t = Serial.readString(); // 读取串口数据. 数据就是目标角度.
    t.trim();                       // 删首尾空格与
    Serial.println(t);              // 串口回显转动的角度.
    go(t.toFloat());
  }
}
