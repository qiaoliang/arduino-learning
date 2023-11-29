#ifndef _SYSCONFIG_H_
#define _SYSCONFIG_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h> // 参见 https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html

const String WIFI_CONFIG_FILE = "wifi_config.json";
const size_t capacity = (unsigned int)2048;
class SysConfig
{
private:
  String AP_SSID;
  String AP_PSK;
  String STA_SSID;
  String STA_PSK;
  int is_FS_ready; // 0:未初始化 1:初始化完成  -1: FS挂载失败

public:
  SysConfig(/* args */);
  ~SysConfig();
  bool LoadWiFiConfig();
  bool SaveWiFiConfig(String STA_SSID, String STA_PSK, String AP_SSID, String AP_PSK);
  String getAP_SSID();
  String getAP_PSK();
  String getSTA_SSID();
  String getSTA_PSK();
  bool isFsAvailable();
  String ListFiles();
};

SysConfig::SysConfig(/* args */)
{
  // 默认值
  AP_SSID = "ESP8266"; // ESP8266自已创建的热点名称
  AP_PSK = "12345678"; // 密码

  STA_SSID = "YOURWIFINAME"; // 路由器WIFI或手机电脑的移动热点名称
  STA_PSK = "YOURWIFIPWD";   // 热点密码
}

SysConfig::~SysConfig()
{
}
bool SysConfig::isFsAvailable()
{
  if (is_FS_ready == 0)
  {
    is_FS_ready = LittleFS.begin() ? 1 : -1;
  }
  return is_FS_ready == 1;
}
bool SysConfig::SaveWiFiConfig(String STA_SSID, String STA_PSK, String AP_SSID, String AP_PSK)
{
  if (!isFsAvailable())
  {
    Serial.println("Failed to mount FS. So it will use default config.");
    return false;
  }
  if (!LittleFS.exists(WIFI_CONFIG_FILE))
  {
    Serial.println(WIFI_CONFIG_FILE + " not exist.");
    return false;
  }
  DynamicJsonDocument doc(2048);
  doc["STA_SSID"] = STA_SSID;
  doc["STA_PSK"] = STA_PSK;
  doc["AP_SSID"] = AP_SSID;
  doc["AP_PSK"] = AP_PSK;
  Serial.println("Saving WiFi config to file.");
  File F = LittleFS.open(WIFI_CONFIG_FILE, "w"); // 创建重写文件
  serializeJson(doc, F);                         // 输出JSON格式内容到文件
  serializeJson(doc, Serial);                    // 输出JSON格式内容到串口
  F.flush();
  F.close(); // 关闭文件
  doc.clear();
  Serial.println("WiFi config loaded.");
  return true;
}
String SysConfig::ListFiles()
{
  String ret;
  FSInfo info;         // 信息
  LittleFS.info(info); // 取闪存文件系统信息
  ret = "闪存.已用:" + String(info.usedBytes) + "字节;可用:" + String(info.totalBytes) + "字节;\n";
  Dir dir = LittleFS.openDir("/"); // 打开根目录,枚举文件名
  while (dir.next())
  { // 循环所有对象
    ret += dir.fileName();
    File F = dir.openFile("r");
    ret += "\t";             // Tab 制表符
    ret += String(F.size()); // 取文件长度
    ret += "\n";
    F.close();
  }
  return ret;
}
bool SysConfig::LoadWiFiConfig()
{
  if (!isFsAvailable())
  {
    Serial.println("Failed to mount FS. So it will use default config.");
    return false;
  }
  if (!LittleFS.exists(WIFI_CONFIG_FILE))
  { // 如果不存在配置文件,使用默认配置参数,并且写入配置文件中.
    Serial.println(WIFI_CONFIG_FILE+" not exist.");
    return false;
  }

  // 如果存在配置文件,从文件中读取配置参数
  File F = LittleFS.open(WIFI_CONFIG_FILE, "r"); // 打开读取配置文件
  F.seek(0);                                     // 到首位置
  String S = F.readString();                     // 读入文本
  F.close();                                     // 关闭文件
  Serial.println(WIFI_CONFIG_FILE+" exist. Using Existed Configuration on Chip.");
  DynamicJsonDocument doc(2048);    // 堆内存JSON文档对象
  deserializeJson(doc, S);          // 把内容装载到JSON对象
  AP_SSID = String(doc["AP_SSID"]); // 读取配置参数到变量
  AP_PSK = String(doc["AP_PSK"]);
  STA_SSID = String(doc["STA_SSID"]);
  STA_PSK = String(doc["STA_PSK"]);
  doc.clear();
  return true;
}
String SysConfig::getAP_SSID()
{
  return AP_SSID;
}
String SysConfig::getAP_PSK()
{
  return AP_PSK;
}
String SysConfig::getSTA_SSID()
{
  return STA_SSID;
}
String SysConfig::getSTA_PSK()
{
  return STA_PSK;
}
#endif
