#ifndef _FEEDCONFIG_H_
#define _FEEDCONFIG_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h> // 参见 https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html

const String WIFI_CONFIG_FILE = "/wifi_config.json";
const size_t capacity = (unsigned int) 2048;
class FeedConfig
{
private:
  String AP_SSID;
  String AP_PSK;
  String STA_SSID;
  String STA_PSK;
  int is_FS_ready; // 0:未初始化 1:初始化完成  -1: FS挂载失败
  bool isFsAvailable();

public:
  FeedConfig(/* args */);
  ~FeedConfig();
  bool LoadWiFiConfig();
  bool SaveWiFiConfig();
  String getAP_SSID();
  String getAP_PSK();
  String getSTA_SSID();
  String getSTA_PSK();
};

FeedConfig::FeedConfig(/* args */)
{
  // 默认值
  AP_SSID = "ESP8266"; // ESP8266自已创建的热点名称
  AP_PSK = "12345678"; // 密码

  STA_SSID = "YOURWIFINAME"; // 路由器WIFI或手机电脑的移动热点名称
  STA_PSK = "YOURWIFIPWD";   // 热点密码
}

FeedConfig::~FeedConfig()
{
}
bool FeedConfig::isFsAvailable()
{
  if(is_FS_ready == 0){
    is_FS_ready = LittleFS.begin()?1;-1;
  }
  return is_FS_ready == 1;
}
bool FeedConfig::SaveWiFiConfig()
{
  if (!isFsAvailable())
  {
    Serial.println("Failed to mount FS. So it will use default config.");
    return false;
  }
  
}

bool FeedConfig::LoadWiFiConfig()
{
  if (!isFsAvailable())
  {
    Serial.println("Failed to mount FS. So it will use default config.");
    return false;
  }
  if (LittleFS.exists(WIFI_CONFIG_FILE))
  {                                           // 如果存在配置文件,从文件中读取配置参数
    File F = LittleFS.open(WIFI_CONFIG_FILE, "r"); // 打开读取配置文件
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
    return false;
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
