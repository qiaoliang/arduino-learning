#include <Arduino.h>
#include <ESP8266WiFi.h>              //编译此代码需要先安装ESP8266开发板文件包,并且只能上传到ESP8266芯片的开发板才能运行.
#include <ESP8266WebServer.h>         //小型HTTP网页服务
#include "LittleFS.h"

String AP_SSID = "ESP8266";  // ESP8266自已创建的热点名称
String AP_PSK = "12345678";  // ESP8266自已Wifi热点的密码


String STA_SSID = "MYHOME";  // 这里写你家路由器WIFI或手机电脑的移动热点名称
String STA_PSK = "12345678"; // 这里写你家WIFI或手机热点密码


void setup()
{
  Serial.begin(115200);           // 开启串口通信 波特率115200,串口监视器也要相同波特率,不然会乱码
  pinMode(LED_BUILTIN, OUTPUT);   // 初始化8266开发板LED信号灯的GPIO口为输出.
  digitalWrite(LED_BUILTIN, LOW); // Mini ESP8266板LED_BUILTIN=GPIO 2,LOW=亮灯,HIGH=灭灯

  WiFi.mode(WIFI_AP_STA);        // 开启WIFI功能,同时开启热点功能.
  if (STA_SSID != "" && STA_PSK != "")
  {
    Serial.printf("\n连接WIFI.SSID:%S 密码:%S\n", STA_SSID, STA_PSK); // 输出要连到的热点
    WiFi.begin(STA_SSID, STA_PSK);                                    // 开发板连 路由器, 手机或电脑创建WIFI无线热点
    for (int i = 0; i < 15; i++)
    { // 循环十五次共15秒.若连网成功就跳出循环
      if (WiFi.status() == WL_CONNECTED)
      {                                     // 成功连网
        IPAddress IP = WiFi.localIP();      // 获取 DHCP 分配的随机IP地址 192.168.X.X
        String S = IP.toString();           // 转为字符串IP地址
        Serial.printf("IP: %s", S.c_str()); // 输出连网得到的IP地址
                                            // 很多手机做移动热点时不显示IP地址
        File F = LittleFS.open("/ip.txt", "w"); //"w" 重写文件所有内容
        F.print(S);
        F.close();           // 保存IP到文件可供查阅;关闭文件
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

  //----------------创建WIFI热点.手机电脑连这个热点后打开 http://192.168.2.1/ ------------
  if (WiFi.status() != WL_CONNECTED)
    WiFi.mode(WIFI_AP);                                                      // 如果连网失败改为纯AP模式
  WiFi.softAPConfig({192, 168, 2, 1}, {192, 168, 2, 1}, {255, 255, 255, 0}); // 配置开发板IP,网关与子网掩码.
  if (WiFi.softAP(AP_SSID, AP_PSK))
  { // 开启WIFI_AP无线网络,热点 与 密码
    Serial.printf("\n创建WIFI.SSID:%S", WiFi.softAPSSID());
    Serial.printf(" 密码:%S", WiFi.softAPPSK());
    Serial.printf(" IP:%s", WiFi.softAPIP().toString().c_str());
  }
  digitalWrite(LED_BUILTIN, LOW); // 亮灯
}

void loop()
{
}
