#ifndef WEBSERVICE_H
#define WEBSERVICE_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h> //小型HTTP网页服务
#include <ESP8266HTTPUpdateServer.h> //寄生网页服务,接受 固件.bin 或 系统.bin http://X.X.X.X/upbin Firmware:固件,FileSystem:文件系统

#include <LittleFS.h>
#include <ArduinoJson.h>
#include "sysConfig.h"
#include "Feeding.h"

ESP8266WebServer server(80); // 建立Web服务对象,HTTP端口80

ESP8266HTTPUpdateServer Updater;     // ESP8266 网络[更新固件]服务

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

String getUploadPage(){
    String html = "<!DOCTYPE html>";
    html += "<html lang='zh-CN'>";
    html += "<head>";
    html += "  <meta  charset='UTF-8'>";
    html += "  <title>五零小白硬件实验室</title>";
    html += "</head>";
    html += "<body>";
    html += "  <center>";
    html += "    <h1>ESP8266 LittleFS 文件上传</h1>";
    html += "    <form method='POST' enctype='multipart/form-data'>";
    html += "      <input type='file' name='data' multiple>";
    html += "      <input class='button' type='submit' value='上传'>";
    html += "    </form>";
    html += "  </center>";
    html += "</body>";
    html += "</html>";
    return html;
}

void handleUserRequest()
{
    String url = server.uri(); // 获取用户请求网址信息
    String ret;
    Serial.print(" 用户请求 ");
    Serial.println(url);
    if (url.equalsIgnoreCase("/upload.html") || url.equalsIgnoreCase("/upload"))
    {
        ret = getUploadPage();
        server.send(200, "text/html", ret);
        return;
    }

    if(url.equalsIgnoreCase("/help.html") || url.equalsIgnoreCase("/help")){
        help();
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
        server.send(200, "text/plain", "没有参数,返回当前配置. "+currentSettings());
        return;
    }


    for (int i = 0; i < server.args(); i++)
    {
        String t = server.argName(i); // 获取参数名
        Serial.print("HTTP指令:");
        Serial.println(t);
        if (t.equalsIgnoreCase("setting"))
        {
            DynamicJsonDocument doc(2048); // 堆内存JSON文档对象
            deserializeJson(doc, server.arg(i)); // 把内容装载到JSON对象
            sysCfg.SaveFeedSetting(doc["C"], doc["S"],doc["I"]);
            goFeed();
            message += "保存当前设置成功.\n并马上执行一次投喂.\n";
            break;
        }
        if(t.equalsIgnoreCase("FeedNow")){
            message += goFeed() + "\n";
        }
    }
    message += "当前的投喂设置是: "+ currentSettings() + "\n";
    server.send(200, "text/plain", message);
}

void handleFileUpload()
{
    static File F; // 静态变量 文件对象用于文件上传

    HTTPUpload &UP = server.upload(); // 上传对象
    if (UP.status == UPLOAD_FILE_START)
    {                                  // 如果 状态=UPLOAD_FILE_START 开始上传
        String filename = UP.filename; // 上传的文件名
        if (!filename.startsWith("/"))
            filename = "/" + filename;  // 为文件名前加上"/"
        F = LittleFS.open(filename, "w"); // 在SPIFFS中建立W重写文件用于写入用户上传的文件数据
    }
    if (UP.status == UPLOAD_FILE_WRITE)
    { // 如果上传状态为UPLOAD_FILE_WRITE
        if (F)
            F.write(UP.buf, UP.currentSize); // 向LittleFS文件写入浏览器发来的文件数据 2048
    }
    if (UP.status == UPLOAD_FILE_END)
    {              // 如果上传状态为UPLOAD_FILE_END
        F.close(); // 关闭文件,完成上传
    }
    if (UP.status == UPLOAD_FILE_ABORTED)
    { // 中断或取消上传
        F.close();
    }
}

void FileUpload_OK()
{

    HTTPUpload &UP =server.upload();
    String html = "<!DOCTYPE html>";
    html += "<html lang='zh-CN'>";
    html += "<head>";
    html += "  <meta  charset='UTF-8'>";
    html += "  <title>五零小白硬件实验室</title>";
    html += "</head>";
    html += "<body>";
    html += "  <center>";
    html += "    <h1>上传文件成功.Size: " + String(UP.contentLength) + "</h1>"; // 文件成功保存
    html += "    <a href='/index.html'>[首页]</a>";
    html += "    <a href='/upload.html'>[继续上传]</a>";
    html += "  </center>";
    html += "</body>";
    html += "</html>";
    server.send(200, "text/html", html);
}

void WEB_Start()
{
    server.on("/command", HTTP_GET, command); // 响应用户的请求
    server.on("/feed", HTTP_GET, goFeed);     // 响应用户的请求
    server.on("/help", HTTP_GET, help);
    server.on("/", HTTP_GET, setting); // 响应用户的请求
    server.on("/index", HTTP_GET, setting);
    server.onNotFound(handleUserRequest); // 对于未定义请求统统让 handleUserRequest 处理
    server.on("/upload.html",                // 上传文件upload页面.   http://IP/upload.html 可以上传 index.html 等网页文件
           HTTP_POST,                     // POST方式向服务器上传文件
           FileUpload_OK,                 // 回复状态码OK=200给客户端  //[](){Web.send(200);}
           handleFileUpload);             // 并且运行处理文件上传函数
    Updater.setup(&server, "/upbin", "admin", "12345678"); // 开启网络更新固件服务,帐密登陆 http://IP/upbin 第一项Firmware固件更新
    server.begin(80);                     // 启动WEB服务器
}
#endif