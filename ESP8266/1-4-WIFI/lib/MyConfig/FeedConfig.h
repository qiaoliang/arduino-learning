#ifndef _FEEDCONFIG_H_
#define _FEEDCONFIG_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h> // 参见 https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html

const String CONFIG_FILE = "/config.json";
const size_t capacity = (unsigned int) 2048;
class FeedConfig
{
private:
  String AP_SSID;
  String AP_PSK;
  String STA_SSID;
  String STA_PSK;
  bool isFsAvailable = false;

public:
  FeedConfig(/* args */);
  ~FeedConfig();
  void Init();
  String getAP_SSID();
  String getAP_PSK();
  String getSTA_SSID();
  String getSTA_PSK();
};

FeedConfig::FeedConfig(/* args */)
{
  AP_SSID = "ESP8266"; // ESP8266自已创建的热点名称
  AP_PSK = "12345678"; // 密码

  STA_SSID = "YOURWIFINAME"; // 路由器WIFI或手机电脑的移动热点名称
  STA_PSK = "YOURWIFIPWD";   // 热点密码
}

FeedConfig::~FeedConfig()
{
}
void FeedConfig::Init()
{

  isFsAvailable = LittleFS.begin();
  if(!isFsAvailable){
    Serial.println("Failed to mount FS. So it will use default config.");
    return;
  }
  if (LittleFS.exists(CONFIG_FILE))
  {                                           // 如果存在配置文件,从文件中读取配置参数
    File F = LittleFS.open(CONFIG_FILE, "r"); // 打开读取配置文件
    F.seek(0);                                // 到首位置
    String S = F.readString();                // 读入文本
    F.close();                                // 关闭文件
    Serial.println("config.json exist. Using Existed Configuration on Chip.");
    DynamicJsonDocument doc(2048);    // 堆内存JSON文档对象
    deserializeJson(doc, S);          // 把内容装载到JSON对象
    AP_SSID = String(doc["AP_SSID"]); // 读取配置参数到变量
    AP_PSK = String(doc["AP_PSK"]);
    STA_SSID = String(doc["STA_SSID"]);
    STA_PSK = String(doc["STA_PSK"]);
    doc.clear();
  }
  else
  { // 如果不存在配置文件,使用默认配置参数,并且写入配置文件中.
    Serial.println("config.json not exist. Using default Configuration.");
    DynamicJsonDocument doc(2048);            // 创建堆内存JSON文档对象
    doc["AP_SSID"] = AP_SSID;                 // 控制板自己创建的WIFI热点
    doc["AP_PSK"] = AP_PSK;                   // 控制板自己的WIFI密码
    doc["STA_SSID"] = STA_SSID;               // 控制板要连接的路由器热点
    doc["STA_PSK"] = STA_PSK;                 // 控制板要连接的路由器密码
    File F = LittleFS.open(CONFIG_FILE, "w"); // 创建重写文件
    serializeJson(doc, F);                    // 输出JSON格式内容到文件
    serializeJson(doc, Serial);               // 输出JSON格式内容到串口
    F.flush();
    F.close(); // 关闭文件
    doc.clear();
  }
}
String FeedConfig::getAP_SSID()
{
  return AP_SSID;
}
String FeedConfig::getAP_PSK()
{
  return AP_PSK;
}
String FeedConfig::getSTA_SSID()
{
  return STA_SSID;
}
String FeedConfig::getSTA_PSK()
{
  return STA_PSK;
}

#endif
