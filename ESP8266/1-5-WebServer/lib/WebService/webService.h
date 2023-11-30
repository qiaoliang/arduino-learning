#ifndef WEBSERVICE_H
#define WEBSERVICE_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h> //小型HTTP网页服务
#include <LittleFS.h>
#include <ArduinoJson.h>

ESP8266WebServer server(80); // 建立Web服务对象,HTTP端口80
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
    if (server.args() == 0)
    {
        if (WiFi.status() == WL_CONNECTED)
        { // 成功连网
            IPAddress IP = WiFi.localIP();
            message = IP.toString() + "\n";
        }
        message += currentSettings();
    }

    static String date; // 静态变量时间戳

    for (int i = 0; i < server.args(); i++)
    {                                 // http://ip/command?x90&y90
        String t = server.argName(i); // 提取一个指令
        if (t.equalsIgnoreCase("time"))
        {
            if (date != server.arg(i))
                date = server.arg(i);
            continue;
        }
        Serial.println(t);

        t.replace("_", "+"); // 把"_"恢复为"+"; 网页脚本arg=arg.replace(/\+/g ,"_"); //把'+'替换为'_'

        Serial.print("HTTP指令:");
        Serial.println(t);
        message += "\n" + t + "\n";
        // message += Command(t); // 解析 并 执行 命令
    }

    server.send(200, "text/plain", message);
}

void responseWithFile(String filename)
{
    if (LittleFS.exists(filename))
    {                                                   // 测试文件是否存在
        File file = LittleFS.open("index.html", "r");   // "r"=只读打开文件
        server.streamFile(file, getType("index.html")); // 发送文件给浏览器
        file.close();                                   // 关闭文件
    }
    else
    {
        server.send(404, "text/plain", "404 Not Found"); // 如果没有匹配的URL,则回复404 Not Found
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
void handleUserRequest()
{
    String url = server.uri(); // 获取用户请求网址信息

    Serial.print(" 用户请求 ");
    Serial.println(url);

    if (url.endsWith("/"))
    {                        // 如果访问地址以"/"为结尾
        url = "/index.html"; // 将访问地址修改为/index.html文件
        if (!LittleFS.exists(url))
        { // 还未上传首页文件跳转 文件上传 页
            server.sendHeader("Location", "/upload.html");
            server.send(303); // 303跳转
            return;
        }
    }
    server.send(404, "text/plain", "404 Not Found"); // 如果没有匹配的URL,则回复404 Not Found
}
void WEB_Start()
{
    server.on("/command", HTTP_GET, command); // 响应用户的请求
    server.on("/help", HTTP_GET, help);
    server.on("/", HTTP_GET, setting); // 响应用户的请求
    server.on("/index", HTTP_GET, setting);
    server.onNotFound(handleUserRequest); // 对于未定义请求统统让 handleUserRequest 处理
    server.begin(80);                     // 启动WEB服务器
}
#endif