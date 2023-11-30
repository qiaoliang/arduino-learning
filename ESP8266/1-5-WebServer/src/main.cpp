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
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h> //小型HTTP网页服务

#include "SysConfig.h"
#include "WebService.h"
#include "WiFi.h"

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

  sysCfg.LoadWiFiConfig();         // 读取配置文件

  WiFi_Init(sysCfg.getSTA_SSID(), sysCfg.getSTA_PSK(), sysCfg.getAP_SSID(), sysCfg.getAP_PSK());      // 初始化WIFI网络,连接路由器或手机热点
  WEB_Start();  // 启动WEB服务
}

void loop()
{
  webService->handleClient(); // 处理客户HTTP访问,上传文件,更新固件等
}
