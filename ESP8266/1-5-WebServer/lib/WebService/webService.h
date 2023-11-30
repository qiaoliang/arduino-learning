#ifndef WEBSERVICE_H
#define WEBSERVICE_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h> //小型HTTP网页服务
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "sysConfig.h"
#include "Feeding.h"

ESP8266WebServer server(80); // 建立Web服务对象,HTTP端口80
extern SysConfig sysCfg;

void command();
void index();
void help();
void handleUserRequest();

ESP8266WebServer *GetWebServer()
{
    return &server;
}
String getType(String filename)
{ // 获取文件类型
    if (filename.endsWith(".htm"))
        return "text/html";
    else if (filename.endsWith(".html"))
        return "text/html";
    else if (filename.endsWith(".css"))
        return "text/css";
    else if (filename.endsWith(".js"))
        return "application/javascript";
    else if (filename.endsWith(".png"))
        return "image/png";
    else if (filename.endsWith(".gif"))
        return "image/gif";
    else if (filename.endsWith(".jpg"))
        return "image/jpeg";
    else if (filename.endsWith(".ico"))
        return "image/x-icon";
    else if (filename.endsWith(".xml"))
        return "text/xml";
    else if (filename.endsWith(".pdf"))
        return "application/x-pdf";
    else if (filename.endsWith(".zip"))
        return "application/x-zip";
    else if (filename.endsWith(".gz"))
        return "application/x-gzip";
    else if (filename.endsWith(".txt"))
        return "text/plain;charset=UTF-8";
    else if (filename.endsWith(".json"))
        return "text/plain;charset=UTF-8";
    return "text/plain";
}
String currentSettings()
{                                // 返回Json格式的当前的设置
    StaticJsonDocument<256> doc; // 栈内存JSON文档对象
    doc["C"] = sysCfg.getCount();
    doc["S"] = sysCfg.getSpeed();
    doc["I"] = sysCfg.getInterval();
    String ret;
    serializeJson(doc, ret); // 单行格式 到变量
    doc.clear();
    return ret; // 格式为{"C":1,"S":1,"I":1}
}
void responseWithFile(String filename)
{
    if (LittleFS.exists(filename))
    {                                               // 测试文件是否存在
        File file = LittleFS.open(filename, "r");   // "r"=只读打开文件
        server.streamFile(file, getType(filename)); // 发送文件给浏览器
        file.close();                               // 关闭文件
    }
    else
    {
        // server.send(404, "text/plain", "404 Not Found"); // 如果没有匹配的URL,则回复404 Not Found
        server.sendHeader("Location", "/upload.html");
        server.send(303); // 303跳转
        return;
    }
}
void setting()
{
    responseWithFile("index.html");
}

void help()
{
    responseWithFile("HELP.html");
}

String goFeed()
{
    FeedNow();
    return "已投喂,请等待一下.";
}


void handleUserRequest()
{
    String url = server.uri(); // 获取用户请求网址信息

    Serial.print(" 用户请求 ");
    Serial.println(url);

    if (url.endsWith("/"))
    {              // 如果访问地址以"/"为结尾
        setting(); // 返回设置页面
        return;
    }
    server.send(404, "text/plain", "404 Not Found"); // 如果没有匹配的URL,则回复404 Not Found
}
void parseSetting(String value){


}
void command()
{ // 网址里不能用 + 符号,会变成空格
    String message = "";
    if (server.args() == 0) // 如果没有参数,则返回当前设置
    {
        Serial.println("HTTP指令:没有参数");
        server.send(200, "text/plain", "没有参数"+currentSettings());
        return;
    }

    static String date; // 静态变量时间戳
    for (int i = 0; i < server.args(); i++)
    {                                 // http://ip/command?x90&y90
        String t = server.argName(i); // 提取一个指令
        Serial.print("HTTP指令:");
        Serial.println(t);
        if (t.equalsIgnoreCase("setting"))
        {
            DynamicJsonDocument doc(2048); // 堆内存JSON文档对象
            deserializeJson(doc, server.arg(i)); // 把内容装载到JSON对象
            sysCfg.SaveFeedSetting(doc["C"], doc["S"],doc["I"]);
            Serial.println("Feed Setting Saved." + currentSettings());
            // 调制好以后, 马上执行喂鱼.
            message+=goFeed()+"\n";
            break;
        }
        if(t.equalsIgnoreCase("FeedNow")){
            message += goFeed() + "\n";
        }
    }
    message += currentSettings() + "\n";
    server.send(200, "text/plain", message);
}

void WEB_Start()
{
    server.on("/command", HTTP_GET, command); // 响应用户的请求
    server.on("/feed", HTTP_GET, goFeed);     // 响应用户的请求
    server.on("/help", HTTP_GET, help);
    server.on("/", HTTP_GET, setting); // 响应用户的请求
    server.on("/index", HTTP_GET, setting);
    server.onNotFound(handleUserRequest); // 对于未定义请求统统让 handleUserRequest 处理
    server.begin(80);                     // 启动WEB服务器
}
#endif