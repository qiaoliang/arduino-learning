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
#include "SysConfig.h"
#include "Feeding.h"
#include "WiFi.h"
#include "WebService.h"

SysConfig sysCfg;

String AP_SSID;
String AP_PSK;

String STA_SSID;
String STA_PSK;

ESP8266WebServer *webService = GetWebServer(); // 建立Web服务对象,HTTP端口80

void setup()
{
  Serial.begin(115200); // 开启串口通信 波特率115200,串口监视器也要相同波特率,不然会乱码
  while (!Serial)
    ;
  pinMode(LED_BUILTIN, OUTPUT);   // 初始化8266开发板LED信号灯的GPIO口为输出.
  digitalWrite(LED_BUILTIN, LOW); // Mini ESP8266板LED_BUILTIN=GPIO 2,LOW=亮灯,HIGH=灭灯

  sysCfg.LoadWiFiConfig();  // 读取WiFi 的配置文件
  if(!sysCfg.LoadFeedSetting()) // 读取喂鱼设置的配置文件
  {
    sysCfg.SaveFeedSetting(); // 如果没有配置文件,则创建默认配置文件
  }
  Serial.println("Feed Setting: " + currentSettings()); // 输出当前的喂鱼设置

  WiFi_Init(sysCfg.getSTA_SSID(), sysCfg.getSTA_PSK(), sysCfg.getAP_SSID(), sysCfg.getAP_PSK()); // 初始化WIFI网络,连接路由器或手机热点
  WEB_Start();     // 启动WEB服务
  delay(5000);
  Servo_Init();
  FeedNow();
}

void loop()
{
  keepRunning();
  webService->handleClient(); // 处理客户HTTP访问,上传文件,更新固件等
}
