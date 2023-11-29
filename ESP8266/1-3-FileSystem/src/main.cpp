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
#include "SysConfig.h"

SysConfig sysConfig;

String AP_SSID;
String AP_PASSWORD;

String STA_SSID;
String STA_PASSWORD;
int counter = 0;
void setup()
{
  Serial.begin(115200); // 开启串口通信 波特率115200,串口监视器也要相同波特率,不然会乱码
  while (!Serial)
    ;

  pinMode(LED_BUILTIN, OUTPUT);   // 初始化8266开发板LED信号灯的GPIO口为输出.
  digitalWrite(LED_BUILTIN, LOW); // Mini ESP8266板LED_BUILTIN=GPIO 2,LOW=亮灯,HIGH=灭灯

  if (!sysConfig.LoadWiFiConfig())
  {
    Serial.println("main: Can NOT load wifi config from data file.");
  }
  else
  {
    STA_SSID = sysConfig.getSTA_SSID();
    STA_PASSWORD = sysConfig.getSTA_PSK();
    AP_SSID = sysConfig.getAP_SSID();
    AP_PASSWORD = sysConfig.getAP_PSK();
    Serial.println("main: init wifi config loaded from data file.");
  }
}

void loop()
{
  if (counter++ < 1)
  {
    if (sysConfig.isFsAvailable())
    {
      Serial.println("main: fs available. ");
      Serial.println("main: fs dir: " + sysConfig.ListFiles());
      Serial.println("STA_SSID : "+STA_SSID);
      Serial.println("STA_PASSWORD : "+STA_PASSWORD);
      Serial.println("AP_SSID : "+AP_SSID);
      Serial.println("AP_PASSWORD : "+AP_PASSWORD);
    }
  }
}
