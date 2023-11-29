
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h> //小型HTTP网页服务

String AP_SSID = "ESP8266"; // ESP8266自已创建的热点名称
String AP_PSK = "12345678"; // 热点密码

String STA_SSID = "HUAWEI-P1031F"; // 路由器WIFI或手机电脑的移动热点名称
String STA_PSK = "5x3abxh7";       // 热点密码

ESP8266WebServer Web(80); // 建立Web服务对象,HTTP端口80

void command()
{
  Web.send(200, "text/plain", "Hello world"); // 回复HTTP请求
}
void handleUserRequest()
{

  Web.send(404, "text/plain", "404 Not Found"); // 如果没有匹配的URL,则回复404 Not Found
}
//---------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200); // 开启串口通信 波特率115200,串口监视器也要相同波特率,不然会乱码
  while (!Serial)
    ;
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

      delay(500);                      // 延时500毫秒
    }
  }


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

  //--------------开启WEB网页服务器.支持网页控制------------------------------------------
  Web.on("/", HTTP_GET, command);    // 响应用户的请求
  Web.onNotFound(handleUserRequest); // 对于未定义请求统统让 handleUserRequest 处理
  Web.begin(80);                     // 启动WEB服务器
}

void loop()
{
  Web.handleClient(); // 处理客户HTTP访问,上传文件,更新固件
}
