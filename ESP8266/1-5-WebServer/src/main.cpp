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

SysConfig sysConfig;

String AP_SSID;
String AP_PSK;

String STA_SSID;
String STA_PSK;
int counter = 0;

ESP8266WebServer webService(80); // 建立Web服务对象,HTTP端口80
String currentSettings()
{                              // 返回Json格式的当前的设置
  StaticJsonDocument<256> doc; // 栈内存JSON文档对象
  doc["C"] = "1";
  doc["S"] = "1";
  doc["I"] = "1";
  String ret;
  serializeJson(doc, ret); // 单行格式 到变量
  doc.clear();
  return ret; //{"C":1,"S":1,"I":1}
}

void command()
{ // 网址里不能用 + 符号,会变成空格
  String message = "";
  if (webService.args() == 0)
  {
    if (WiFi.status() == WL_CONNECTED)
    { // 成功连网
      IPAddress IP = WiFi.localIP();
      message = IP.toString() + "\n";
    }
    message += currentSettings();
  }

  static String date; // 静态变量时间戳

  for (int i = 0; i < webService.args(); i++)
  {                                   // http://ip/command?x90&y90
    String t = webService.argName(i); // 提取一个指令
    if (t.equalsIgnoreCase("time"))
    {
      if (date != webService.arg(i))
        date = webService.arg(i);
      continue;
    }
    Serial.println(t);

    t.replace("_", "+"); // 把"_"恢复为"+"; 网页脚本arg=arg.replace(/\+/g ,"_"); //把'+'替换为'_'

    Serial.print("HTTP指令:");
    Serial.println(t);
    message += "\n" + t + "\n";
    // message += Command(t); // 解析 并 执行 命令
  }

  webService.send(200, "text/plain", message);
}
void handleUserRequest()
{
  String url = webService.uri(); // 获取用户请求网址信息

  Serial.print(" 用户请求 ");
  Serial.println(url);

  if (url.endsWith("/"))
  {                      // 如果访问地址以"/"为结尾
    url = "/index.html"; // 将访问地址修改为/index.html文件
    if (!LittleFS.exists(url))
    { // 还未上传首页文件跳转 文件上传 页
      webService.sendHeader("Location", "/upload.html");
      webService.send(303); // 303跳转
      return;
    }
  }
  webService.send(404, "text/plain", "404 Not Found"); // 如果没有匹配的URL,则回复404 Not Found
}

void Enable_STA_AND_AP(){
  //-----------------------下面是连接WIFI网络.可以连路由器或电脑与手机的创建的热点网络--------------------
  WiFi.mode(WIFI_AP_STA); // WIFI_STA=客户端模式，WIFI_AP=热点模式
  if (STA_SSID != "" && STA_PSK != "")
  {
    Serial.println("Connecting WIFI.SSID: " + STA_SSID); // 输出要连到的热点
    Serial.println("Connecting WIFI.PSK: " + STA_PSK);
    WiFi.begin(STA_SSID, STA_PSK); // 开发板连 路由器,手机或电脑创建WIFI无线热点,也可开网页控制机械臂
    for (int i = 0; i < 15; i++)
    { // 循环十五次共15秒.若连网成功就跳出循环
      if (WiFi.status() == WL_CONNECTED)
      {                                // 成功连网
        IPAddress IP = WiFi.localIP(); // 获取 DHCP 分配的随机IP地址 192.168.X.X
        String S = IP.toString();      // 转为字符串IP地址
        Serial.println("IP: " + S);    // 输出连网得到的IP地址
                                       // 很多手机做移动热点时不显示IP地址

        S.replace(".", "-"); // 把 192.168.X.X 转成 192-168-X-X 设为网络主机名称
        WiFi.hostname(S);    // 修改的名称不一定成功显示.多刷新几次手机里已连接设备 查看
        break;               // 连网成功,跳出循环;在同网络里 手机或电脑打开 http://ip/ 就能控制机械臂
      }
      Serial.print(".");
      digitalWrite(LED_BUILTIN, HIGH); // 灭灯
      delay(500);                      // 延时500毫秒
      digitalWrite(LED_BUILTIN, LOW);  // 亮灯
      delay(500);                      // 延时500毫秒
    }
  }
  digitalWrite(LED_BUILTIN, LOW); // 亮灯

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected " + STA_SSID);
    WiFi.mode(WIFI_AP);
  }
  else
  {
    Serial.println("WiFi connected to " + STA_SSID);
  }                                                                            // 如果连网失败改为纯AP模式
  WiFi.softAPConfig({192, 168, 10, 1}, {192, 168, 10, 1}, {255, 255, 255, 0}); // 配置开发板IP,网关与子网掩码.
  if (WiFi.softAP(AP_SSID, AP_PSK))
  { // 开启WIFI_AP无线网络,热点 与 密码
    Serial.println("Creating WIFI.SSID: " + WiFi.softAPSSID());
    Serial.println(" PWD : " + WiFi.softAPPSK());
    Serial.println(" IP :" + WiFi.softAPIP().toString());
    Serial.println(AP_SSID + "Web AP is ready.");
  }
  else
  {
    Serial.println(AP_SSID + " WIFI failed to setup,");
  }
}
void WiFi_Init(){
  if (!sysConfig.LoadWiFiConfig())
  {
    Serial.println("main: Can NOT load wifi config from data file.");
  }
  else
  {
    STA_SSID = sysConfig.getSTA_SSID();
    STA_PSK = sysConfig.getSTA_PSK();
    AP_SSID = sysConfig.getAP_SSID();
    AP_PSK = sysConfig.getAP_PSK();
    Serial.println("main: init wifi config loaded from data file.");
  }
  Enable_STA_AND_AP();
}

void setup()
{
  Serial.begin(115200); // 开启串口通信 波特率115200,串口监视器也要相同波特率,不然会乱码
  while (!Serial)
    ;

  pinMode(LED_BUILTIN, OUTPUT);   // 初始化8266开发板LED信号灯的GPIO口为输出.
  digitalWrite(LED_BUILTIN, LOW); // Mini ESP8266板LED_BUILTIN=GPIO 2,LOW=亮灯,HIGH=灭灯
  WiFi_Init();
  //--------------开启WEB网页服务器.支持网页控制------------------------------------------
  webService.on("/", HTTP_GET, command); // 响应用户的请求
  webService.onNotFound(handleUserRequest); // 对于未定义请求统统让 handleUserRequest 处理
  webService.begin(80);                     // 启动WEB服务器
}




void loop()
{
  webService.handleClient(); // 处理客户HTTP访问,上传文件,更新固件
}
