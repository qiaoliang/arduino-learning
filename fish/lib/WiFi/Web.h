#include <ESP8266WebServer.h>        //小型HTTP网页服务
#include <ESP8266HTTPUpdateServer.h> //寄生网页服务,接受 固件.bin 或 系统.bin http://X.X.X.X/upbin Firmware:固件,FileSystem:文件系统

ESP8266WebServer Web(80);        // 建立Web服务对象,HTTP端口80
ESP8266HTTPUpdateServer Updater; // ESP8266 网络[更新固件]服务

//-------------------------------------------------------------
/**
 * @brief 处理URL 请求的参数
 *
 * @param command  参数
 * @return String  处理后的结果
 */
String Executing(String command){
   // TODO:
   return "command result";
}
/**
     * @brief 用于 /command 的处理
     *
     */
void command()
{ // 网址里不能用 + 符号,会变成空格
    String message = "";
    if (Web.args() == 0)
    { // http://ip/command
        if (WiFi.status() == WL_CONNECTED)
        { // 成功连网
            IPAddress IP = WiFi.localIP();
            message = IP.toString() + "\n";
        }
        message += "你已经成功连接到 ESP8622. \n";
    }

    for (int i = 0; i < Web.args(); i++)
    {                              // http://ip/command?x90&y90
        String t = Web.argName(i); // 提取一个指令
        message += Executing(t); // 解析 并 执行 命令
    }
    Web.send(200, "text/plain", message);
}
//---------------处理用户浏览器的HTTP访问-------------------
void handleUserRequest()
{
    Web.send(404, "text/plain", "404 Not Found"); // 如果没有找到文件,则回复404 Not Found
}
//---------------------------------------------------------------------------
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
void Web_init()
{
    //--------------开启WEB网页服务器.支持网页控制------------------------------------------
    Web.on("/command", HTTP_GET, command);              // 机械臂指令处理回调函数  http://IP/command?x++;Y100;Z90
    Web.onNotFound(handleUserRequest);                  // 对于未定义请求统统让他处理
    Updater.setup(&Web, "/upbin", "admin", "12345678"); // 开启网络更新固件服务,帐密登陆 http://IP/upbin 第一项Firmware固件更新
    Web.begin(80);                                      // 启动WEB服务器
    Serial.println("\nWeb 网页控制服务器开启");
}