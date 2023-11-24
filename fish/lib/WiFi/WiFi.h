
#include <ESP8266WiFi.h>             //编译此代码需要先安装ESP8266开发板文件包,并且只能上传到ESP8266芯片的开发板才能运行.

#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include "Web.h"


String AP_SSID = "ESP8266"; // ESP8266自已创建的热点名称
String AP_PSK = "12345678"; // ESP8266自已Wifi热点的密码

void Logf(String formats, String words){
    Serial.printf(formats.c_str(), words.c_str());
}
void WiFi_Init()
{

    WiFi.mode(WIFI_AP);                            //如果连网失败改为纯AP模式
    WiFi.softAPConfig({ 192, 168, 1, 1 }, { 192, 168, 1, 1 }, { 255, 255, 255, 0 });  //配置开发板IP,网关与子网掩码.
    if (WiFi.softAP(AP_SSID, AP_PSK)) {                                               //开启WIFI_AP无线网络,热点 与 密码
        Logf("\n创建WIFI.SSID:%S", WiFi.softAPSSID());
        Logf(" 密码:%S", WiFi.softAPPSK());
        Logf(" IP:%s", WiFi.softAPIP().toString());
    }
    Web_init();
}