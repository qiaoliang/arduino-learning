#ifndef _WIFI_H_
#define _WIFI_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
/**
 * @brief 连接WIFI网络.可以连路由器或电脑与手机的创建的热点网络
 *
 * @param STA_SSID 路由器或热点的名称
 * @param STA_PSK  路由器或热点的密码
 * @return true 连接成功
 * @return false 连接失败
 */
bool Enable_STA(String STA_SSID, String STA_PSK)
{
    WiFi.mode(WIFI_AP_STA); // WIFI_STA=客户端模式，WIFI_AP=热点模式. WIFI_AP_STA=两者都有
    if (STA_SSID == "" || STA_PSK == "")
        return false;

    Serial.println("Connecting WIFI.SSID: " + STA_SSID); // 输出要连到的热点
    Serial.println("Connecting WIFI.PSK: " + STA_PSK);
    WiFi.begin(STA_SSID, STA_PSK); // 开发板连 路由器,手机或电脑创建WIFI无线热点,也可开网页控制机械臂
    for (int i = 0; i < 15; i++)
    { // 循环十五次共15秒.若连网成功就跳出循环
        if (WiFi.status() == WL_CONNECTED)
        {                                  // 成功连网
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
    return WiFi.status() == WL_CONNECTED;
}
/**
 * @brief ESP8266自己创建热点
 *
 * @param AP_SSID 热点名称
 * @param AP_PSK  热点密码
 * @return true   创建成功
 * @return false  创建失败
 */
bool Enable_AP(String AP_SSID, String AP_PSK)
{                                                        // WIFI_AP=热点模式
    WiFi.mode(WIFI_AP);                                                          // 如果连网失败改为纯AP模式
    WiFi.softAPConfig({192, 168, 10, 1}, {192, 168, 10, 1}, {255, 255, 255, 0}); // 配置开发板IP,网关与子网掩码.
    if (WiFi.softAP(AP_SSID, AP_PSK))
    { // 开启WIFI_AP无线网络,热点 与 密码
        Serial.println("Creating WIFI.SSID: " + WiFi.softAPSSID());
        Serial.println(" PWD : " + WiFi.softAPPSK());
        Serial.println(" IP :" + WiFi.softAPIP().toString());
        Serial.println(AP_SSID + "Web AP is ready.");
        return true;
    }
    else
    {
        Serial.println(AP_SSID + " WIFI failed to setup,");
        return false;
    }
}
/**
 * @brief  下面是连接WIFI网络.可以连路由器或电脑与手机的创建的热点网络
 *
 * @param STA_SSID  路由器或手机热点的名称
 * @param STA_PSK   路由器或手机热点的密码
 * @param AP_SSID   ESP自己创建的热点名称
 * @param AP_PSK    ESP自己创建的热点密码
 */
void WiFi_Init(String STA_SSID, String STA_PSK, String AP_SSID, String AP_PSK)
{
    digitalWrite(LED_BUILTIN, LOW); // 亮灯
    if (!Enable_STA(STA_SSID, STA_PSK))  // 如果无法连接到路由器,再启动ESP自己的热点.
    {
        Serial.println("WiFi not connected " + STA_SSID);
        Enable_AP(AP_SSID, AP_PSK);
    }
    else
    {
        Serial.println("WiFi connected to " + STA_SSID);
    }
}

#endif