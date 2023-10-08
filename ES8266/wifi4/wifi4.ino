#include <Arduino.h>
#include <ESP8266WiFi.h>              //编译此代码需要先安装ESP8266开发板文件包,并且只能上传到ESP8266芯片的开发板才能运行.
#include <ESP8266WebServer.h>         //小型HTTP网页服务
#include <ESP8266HTTPUpdateServer.h>  //寄生网页服务,接受 固件.bin 或 系统.bin http://X.X.X.X/upbin Firmware:固件,FileSystem:文件系统
#include <ArduinoJson.h>  //使用Json文件格式做配置文件
#include "FS.h"           //ESP8266开发板自带4MB闪存空间,可以用来读写存删文件
#include <Servo.h>                                 //引用 舵机 功能库头文件

String AP_SSID = "ESP8266";  //ESP8266自已创建的热点名称
String AP_PSK = "12345678";  //ESP8266自已Wifi热点的密码

String STA_SSID = "MYHOME";   //这里写你家路由器WIFI或手机电脑的移动热点名称
String STA_PSK = "12345678";  //这里写你家WIFI或手机热点密码

bool Response = false;            //应答标记 true false.网页请求应该快速回答,但R指令执行动作文件可能要很久.
ESP8266WebServer Web(80);         //建立Web服务对象,HTTP端口80
ESP8266HTTPUpdateServer Updater;  //ESP8266 网络[更新固件]服务

const int ss = 4;                       //4轴
Servo S[ss];                                  //创建四个舵机对象
char XYZE[ss] = { 'X', 'Y', 'Z', 'E' };       //定义4个电机从底座到夹子为 XYZBE
int pin[ss] = { D2, D3, D0, D8};              //开发板的数字针脚用来接4个电机
int originPos[ss] = { 90, 90, 90, 90};        //舵机的初始角度
int lastPos[ss] = { 90, 90, 90, 90};          //舵机上次转动到的角度
int newPos[ss] = { 90, 90, 90, 90 };          //舵机新的目标角度
int minPos[ss] = { 0, 50, 90, 10 };           //舵机可转动的最小角度
int maxPos[ss] = { 180, 180, 180, 100 };      //舵机可转动的最大角度

bool Step = true;          //true=减速(按 1 度角减速),false=0 舵机原速转动
float factor = 11.11;      //每转一度对应的脉冲宽度，11.11=(2500-500)/180度
volatile int Autorun = 0;  //自动执行Auto脚本的次数， 0=不运行 
String Cmd, Cmdret = "";   //把一些指令放在这个变量,下次loop循环时执行

/*------------------------------
// 把 0-180 角度值转为对应高电平脉冲信号时间宽度（脉冲宽度）
输入：degree， 目标角度值
返回值：输入角度所对应的脉冲宽度，单位是微秒。大于 360时，直接返回，或 500 ， 2500。
--------------------------------------*/
int todms(float degree) {  //返回  把角度值转换为维持高电平的时间长度
  unsigned long pulseWidth;
  if (degree < 360.0) {    //值少于360视为角度,否则，入参被视为时间长度，单位为微秒
    unsigned long pulseWidth = degree * factor;
    if ((float)(pulseWidth - (int)pulseWidth) >= 0.45) degree = degree + 1.0;
    return 500 + (int)pulseWidth;  //500 对应着起点，即 0 度。
  } else {
    pulseWidth = constrain((int)degree, 500, 2500);  //取值500~2500间,9克舵机都对应这个数值。
    return pulseWidth;
  }
}
/*----------------------------------------------------------------------
// 用途：向舵机发送需要转动到的目标角度。
//      如果上一个动作还没有结束，则继续上一个动作。
//      如果是第四个舵机（夹取舵机—），需要限制Y轴与Z轴最大钝角与最小锐角。它们与装配角度有关
// 参数1: servoNo，舵机的序号
// 参数2: Value，舵机转动的角度
// 返回值：
        false，如果是 X，Y，Z 或 E 舵机的话。
---------------------------------------------------------------------*/

bool Servo180(int servoNo, int Value) {  //脉宽高电平 500微秒 到 2500微秒 之间，对应舵机0°～180°可转角度
  //------------------------------------------------------------------------
  if (servoNo >= 0 && servoNo < ss) {         // 如果是X,Y,Z 和 E 舵机的其中之一
    if (newPos[servoNo] != lastPos[servoNo]) {  //如果上次动作还没有完成
      Servo180(-1, 0);             //则立即去执行完
    }
    newPos[servoNo] = constrain(todms(Value), minPos[servoNo], maxPos[servoNo]);  //指定新的目标角度
    if (ss == 4) Servo4(servoNo);
    return false;
  }

  //----------------------------------------------
  int count = 0;                   //对应的角度值
  unsigned long ms = millis();  //记录板子启动到现在的时间
  int J = 0, maxms = 0;
  do {
    J = 0;
    for (int I = 0; I < ss; I++) {                   //检查X，Y，Z，E舵机,找出要转动的
      if (newPos[I] != lastPos[I]) {              //新旧值不同,需要发信号转动
        int range = abs(newPos[I] - lastPos[I]);  //计算当前转到目标的脉宽差
        maxms = max(maxms, range);               //支持多舵机同时转动,记下最长的耗时。
        if (Step && range > (int)factor) {       //需要减速，并且 range 大于 1 度对应的时长，
          if (newPos[I] > lastPos[I]) {
            lastPos[I] += (int)factor;  //正转1度
          } else {
            lastPos[I] -= (int)factor;  //反转1度
          }
          S[I].write(lastPos[I]);  //向对应的引脚发送信号
          delay(1);
        } else {
          S[I].write(newPos[I]);  //原速转动,或者角度没有变化时，直接发送目标脉宽信号
          lastPos[I] = newPos[I];  //保存新的脉宽
        }
      } else J++;
    }
  } while (J < ss); // 直到所有舵机都转到指定位置

  // 9克舵机的空载速度是每2000微秒可以转1度 ，而 2000微秒相当于转11.11 个脉冲宽度(因为每转1个脉宽信号，需要消耗182微秒)
  // 计算需要转动时间最长的舵机是否在当前时间里完成了转动。
  // 如果没有完成，则等待一段时间
  ms = maxms / (int)factor * 2 + ms;
  if (ms > millis()) {  //检查目前已消耗的时间与转到目标角度需要的时间
    delay(constrain(ms - millis(), 0, 400));
  }

  return maxms != 0;  //如果maxms 没有改变，仍旧是0，返回 flase，说明所有舵机都不需要转动。否则，说明有发送过舵机转动指令信号，返回 true
}

/*-------------------------
//四轴机械臂 限制Y轴与Z轴最大钝角与最小锐角,与舵机装配角度有关---
// 参数1: servoNo，舵机的序号
---------------------------*/
void Servo4(int servoNo) {  
  int v = newPos[1] + newPos[2];
  if (servoNo == 1) {      //控制Y舵机的角度
    if (v < 3350) {  //Y+Z 锐角小于3350脉宽,限值3350;约 30度
      newPos[2] += 3350 - v;
      if (newPos[2] > maxPos[2]) newPos[1] += newPos[2] - maxPos[2];
    }
    if (v > 4550)  //Y+Z 钝角大于4550脉宽,限值4550;约140度
      newPos[2] -= v - 4550;
    if (minPos[2] > newPos[2]) newPos[1] -= minPos[2] - newPos[2];
  }
  if (servoNo == 2) {    //控制Z角度
    if (v < 3350)  //Y+Z 锐角小于3350脉宽,限值3350;
      newPos[1] += 3350 - v;
    if (newPos[1] > maxPos[1]) newPos[2] += newPos[1] - maxPos[1];
    if (v > 4550)  //Y+Z 钝角大于4550脉宽,限值4550;
      newPos[1] -= v - 4550;
    if (minPos[1] > newPos[1]) newPos[2] -= minPos[1] - newPos[1];
  }
}

//-----------------------------------------------------------------------
ICACHE_RAM_ATTR void ISR() {
  Autorun = 1;
}  //D1中断事件处理
//-----------------------------------------------------------------------
//------------------开发板通电后初始函数 setup()----------------------------
void setup() {
  Serial.begin(115200);                                     //开启串口通信 波特率115200,串口监视器也要相同波特率,不然会乱码
  pinMode(LED_BUILTIN, OUTPUT);                             //初始化8266开发板LED信号灯的GPIO口为输出.
  digitalWrite(LED_BUILTIN, LOW);                           //Mini ESP8266板LED_BUILTIN=GPIO 2,LOW=亮灯,HIGH=灭灯

  //------------ arduino 菜单->工具->Flash Size->4MB(FS:2MB OTA:1019KB) -----------------
  //ESP8266开发板代码.格式化并建立内置闪存文件系统,用来保存 Auto.txt 等机械臂自动化动作指令
  SPIFFS.format();         //每次使用 格式化SPIFFS,可清除所有内容。
  SPIFFS.begin();     //开启闪存文件系统
  FSInfo info;        //信息
  SPIFFS.info(info);  //取闪存文件系统信息

  loadConfig();  //载入配置文件(如果有),替换一些变量值
  //-------------设置舵机数字插口与脉冲宽度微秒时间,驱动舵机到初始角度---------------
  if (ss == 4 && todms((float)originPos[2]) < 2000) originPos[2] = 2490;  //四轴Z舵机默认在 180度 位置
  for (int I = 0; I < ss; I++) {
    S[I].attach(pin[I], 500, 2500);  //绑定针脚,设置信号脉冲宽度范围//S[I].detach();
    originPos[I] = todms((float)originPos[I]);
    lastPos[I] = originPos[I];  //originPos
    newPos[I] = originPos[I];  //originPos
    minPos[I] = todms((float)minPos[I]);
    maxPos[I] = todms((float)maxPos[I]);
    S[I].write(originPos[I]);  //写入新角度值,控制舵机转动
    delay(500);             //等待该舵机转到目标角度.
  }

  pinMode(D1, OUTPUT);  //设置D1为输出模式
  //---使用D1引脚为脚本启动开关, 当D1与 5V正极接触,中断调用 ISR 设置 Autorun=1 运行 Auto.txt 动作脚本文件-----
  digitalWrite(D1, LOW);                                    //设置D1为低电平 LOW状态
  attachInterrupt(digitalPinToInterrupt(D1), ISR, RISING);  //启用硬件中断实时捕获 D1 引脚低电平LOW变为高电平HIGH

  //-----------------------下面是连接WIFI网络.可以连路由器或电脑与手机的创建的热点网络--------------------
  WiFi.mode(WIFI_AP_STA);  //WIFI_STA=客户端模式，WIFI_AP=热点模式
  if (STA_SSID != "" && STA_PSK != "") {
    Serial.printf("\n连接WIFI.SSID:%S 密码:%S\n", STA_SSID, STA_PSK);  //输出要连到的热点
    WiFi.begin(STA_SSID, STA_PSK);                                     //开发板连 路由器, 手机或电脑创建WIFI无线热点,也可开网页控制机械臂
    for (int i = 0; i < 15; i++) {                                     //循环十五次共15秒.若连网成功就跳出循环
      if (WiFi.status() == WL_CONNECTED) {                             //成功连网
        IPAddress IP = WiFi.localIP();                                 //获取 DHCP 分配的随机IP地址 192.168.X.X
        String S = IP.toString();                                      //转为字符串IP地址
        Serial.printf("IP: %s", S.c_str());     //输出连网得到的IP地址
                                               //很多手机做移动热点时不显示IP地址
        File F = SPIFFS.open("/ip.txt", "w");  //"w" 重写文件所有内容
        F.print(S);
        F.close();  //保存IP到文件可供查阅;关闭文件
        S.replace(".", "-");  //把 192.168.X.X 转成 192-168-X-X 设为网络主机名称
        WiFi.hostname(S);     //修改的名称不一定成功显示.多刷新几次手机里已连接设备 查看
        break;                //连网成功,跳出循环;在同网络里 手机或电脑打开 http://ip/ 就能控制机械臂
      }
      Serial.print(".");
      digitalWrite(LED_BUILTIN, HIGH);  //灭灯
      delay(500);                       //延时500毫秒
      digitalWrite(LED_BUILTIN, LOW);   //亮灯
      delay(500);                       //延时500毫秒
    }
  }
  digitalWrite(LED_BUILTIN, LOW);  //亮灯

  //----------------创建WIFI热点.手机电脑连这个热点后打开 http://192.168.1.1/ 就能控制机械臂------------
  if (WiFi.status() != WL_CONNECTED) WiFi.mode(WIFI_AP);                            //如果连网失败改为纯AP模式
  WiFi.softAPConfig({ 192, 168, 1, 1 }, { 192, 168, 1, 1 }, { 255, 255, 255, 0 });  //配置开发板IP,网关与子网掩码.
  if (WiFi.softAP(AP_SSID, AP_PSK)) {                                               //开启WIFI_AP无线网络,热点 与 密码
    Serial.printf("\n创建WIFI.SSID:%S", WiFi.softAPSSID());
    Serial.printf(" 密码:%S", WiFi.softAPPSK());
    Serial.printf(" IP:%s", WiFi.softAPIP().toString().c_str());
  }

  //--------------开启WEB网页服务器.支持网页控制------------------------------------------
  Web.on("/command", HTTP_GET, command);               //机械臂指令处理回调函数  http://IP/command?x++;Y100;Z90
  Web.on("/upload.html",                               //上传文件upload页面.   http://IP/upload.html 可以上传 index.html 等网页文件
         HTTP_POST,                                    //POST方式向服务器上传文件
         FileUpload_OK,                                //回复状态码OK=200给客户端  //[](){Web.send(200);}
         handleFileUpload);                            //并且运行处理文件上传函数
  Web.onNotFound(handleUserRequest);                   //对于未定义请求统统让他处理
  Web.on("/config", HTTP_GET, Config);                 //机械臂设置处理回调函数  http://IP/config
  Updater.setup(&Web, "/upbin", "admin", "12345678");  //开启网络更新固件服务,帐密登陆 http://IP/upbin 第一项Firmware固件更新
  Web.begin(80);                                       //启动WEB服务器
  Serial.println("\nWeb 网页控制服务器开启");

}
//-----------------------------------------------------------------------
//
//
//
//------------------载入配置文件，修改一些变量数据参数------------------------
void loadConfig() {                           //载入配置文件/config.json替换掉一些变量中的值
  File F = SPIFFS.open("/config.json", "r");  //打开读取配置文件
  F.seek(0);                                  //到首位置
  String S = F.readString();                  //读入文本
  F.close();                                  //关闭文件

  if (S.length() > 32) {
    DynamicJsonDocument doc(2048);  //堆内存JSON文档对象
    deserializeJson(doc, S);        //把内容装载到JSON对象

    AP_SSID = String(doc["AP_SSID"]);  //读取配置参数到变量
    AP_PSK = String(doc["AP_PSK"]);
    STA_SSID = String(doc["STA_SSID"]);
    STA_PSK = String(doc["STA_PSK"]);

    for (int sNo = 0; sNo < ss; sNo++) {      //读取舵机参数到数组变量
      const char* C = doc["Servo"][sNo];  //舵机编号
      XYZE[sNo] = *C;
      pin[sNo] = doc["pin"][sNo];                      //舵机GPIO
      originPos[sNo] = todms(float(doc["originPos"][sNo]));  //原脉宽 1500=居中90度
      minPos[sNo] = todms(float(doc["minPos"][sNo]));  //最小脉宽微秒值
      maxPos[sNo] = todms(float(doc["maxPos"][sNo]));  //最大脉宽微秒值
    }
    Autorun = doc["Autorun"];  //板子通电自动运行Auto.txt次数
    doc.clear();
  }
}
//-------------------------------------------------------------
String output() {               //返回Json格式的所有舵机当前角度信息
  StaticJsonDocument<256> doc;  //创建一个栈内存JSON文档对象

  for (int servoNo = 0; servoNo < ss; servoNo++) {
    String s = String(XYZE[servoNo]);  //舵机编号
    float v = (float)(newPos[servoNo] - 500.0) / factor;
    doc[s] = String(v, 1);  //输出带1位精度的角度值
  }
  String ret;
  serializeJson(doc, ret);  //单行格式 到变量
  doc.clear();
  return ret;  //{"X":90,"Y":90,"Z":90,"E":90,"B":90,"T":90}
}

//-----------------------------------------------------------------
//
//
//
//------------------开发板循环调用函数 loop()------------------------
void loop() {
  Web.handleClient();                       //处理客户HTTP访问,上传文件,更新固件

  if (Autorun != 0) {            //开启循环执行Auto文件命令
    Command("R Auto.txt");       //执行 R 命令
    if (Autorun > 0) Autorun--;  //正数减1次,负数不变
  }
  if (Cmd != "") {        //如果cmd全局变量里有未执行命令,现在执行
    Cmdret = splitAndExecuteCmd(Cmd);  //分析指令并调用Command执行
    Cmd = "";
  }

  //---------------监听电脑串口输入指令-------------------
  if (Serial.available() > 0) {      //有串口数据>0字节
    String t = Serial.readString();  //?SDCHRA XYZBTE
    t.trim();                        //删首尾空格与换行
    Serial.println(t);               //串口回显指令
    Autorun = 0;                     //有命令时停止动作
    Serial.println(splitAndExecuteCmd(t));
  }
}

//---------------------------------------------------
// 舵机控制命令支持 单个执行也支持多个同时执行
// 单命令   X50
// 多命令   X50;Y100;Z100
// 多行命令 X50;Y100\nZ100;E80
//---------------------------------------------------
String splitAndExecuteCmd(String t) {  //拆分多行命令 或以 ; 为分隔的多个命令
  String ret = "";
  long i[2] = { 0 };  //[0]=; [1]=\n
  do {
    i[0] = t.indexOf(";", 0);        //查找分隔符 ;
    i[1] = t.indexOf("\n", 0);       //查找换行符 \n
    if (i[0] == -1 && i[1] == -1) {  //没找到符号
      ret = Command(t);
      break;                                //单个指令执行跳出
    } else if (i[0] != 0 && i[1] == -1) {   //单行有 ; 分隔的多个指令
      ret = Command(t.substring(0, i[0]));  //提取一个指令执行
      t.remove(0, i[0] + 1);                //删除已执行的指令
    } else if (i[1] != 0 && i[0] == -1) {   //多行的 无分隔的单个指令
      ret = Command(t.substring(0, i[1]));  //提取单行指令执行
      t.remove(0, i[1] + 1);                //删除已执行的此行
      if (Servo180(-1, 0)) Serial.println(output());
    } else if (i[0] < i[1]) {               // ; 在 \n 之前
      ret = Command(t.substring(0, i[0]));  //提取一个指令执行
      t.remove(0, i[0] + 1);                //删除已执行指令
    } else if (i[0] > i[1]) {               // \n 在 ; 之前
      ret = Command(t.substring(0, i[1]));  //提取一行指令执行
      t.remove(0, i[1] + 1);                //删除已执行指令
      if (Servo180(-1, 0)) Serial.println(output());
    } else break;
  } while (t.length() > 0);  //未查找完 继续循环
  if (ret == "")
    if (Servo180(-1, 0)) ret = output();
  return ret;
}
//------------------------------------------------------------------------
//-----------------解析并执行命令 已定义常用命令 ?SDCHRA XYZBE----------------
String Command(String t) {
  static int dms[6] = { 0 };         //静态变量,存放上次 S 保存过的电机位置
  static String file = "/Auto.txt";  //静态变量,存放FSDRC命令将操作的文件

  int i = 0, j = 0, v = 0;  //定位指令文本中位置
  String Auto;              //Auto.txt文件内容
  String S = "";            //保存命令所带的参数
  String ret = "";          //返回结果

  //-------------------拆分以空格分隔的命令与参数------------------
  t.trim();               //删首尾空格与换行
  i = t.indexOf(" ", 0);  //查找空格 拆分命令与参数
  if (i > 0) {
    S = t.substring(i + 1);  //提取参数
    S.trim();                //删首尾空格与换行
    t.remove(i);             //保留命令,删除参数
  }

  //----------------解析执行一些命令 ?SDCHRA --------------------------------
  if (t.equalsIgnoreCase("RE")) {  //re 重启开发板
    ESP.restart();                 //软重启
    return "";
  }       
  if (t == "?" || t == "？") {                  //？HELP 输出简要的帮助信息
    if (S == "") S = "/HELP.txt";               //? Auto 输出指定文件的内容
    if (!S.startsWith("/")) S = "/" + S;        //比较字符串前缀
    if (1 > S.indexOf(".", 1)) S = S + ".txt";  //比较字符串后缀
    File F = SPIFFS.open(S, "r");               //"r"=只读内容,"w"=重写内容
    F.seek(0);                                  //到首位置
    ret = F.readString();                       //读入文本
    F.close();                                  //关闭文件
    return ret;
  }
  if (t.startsWith("//")) {  //跳过动作文件中的备注行
    return "";
  }
  if (t.equalsIgnoreCase("F")) {                //用 "F Auto.txt" 改变动作文件名
    if (S == "") S = "/Auto.txt";               //用 "F test.txt" 指定动作文件名
    if (!S.startsWith("/")) S = "/" + S;        //比较字符串前缀
    if (1 > S.indexOf(".", 1)) S = S + ".txt";  //比较字符串后缀
    file = S;                                   //保存文件名 R S D C 将对该文件操作
    return ("{\"F\":\"" + file + "\"}");        //输出新动作文件名称
  }
  if (t.equalsIgnoreCase("delay")) {  //动作脚本中 delay xx 由这段代码执行
    int v = S.toInt();                //delay的参数,即是要延时的毫秒时间
    unsigned long ms = millis();      //板子开机已经过的毫秒时间
    Servo180(-1, 0);
    while (millis() - ms < v) {  //在要延时的毫秒时间里一直循环
      delay(0);
      Web.handleClient();  //处理客户HTTP访问,上传文件,更新固件
    }
    return "";
  }
  if (t.equalsIgnoreCase("SH")) {         //保存XYZE舵机当前位置到 H.txt 文件
    File F = SPIFFS.open("/H.txt", "w");  //"w"=重写文件所有内容
    F.seek(0);                            //到首位置
    for (int I = 0; I < ss; I++) {        //循环所有舵机变量进行输出保存
      float f = (float)(newPos[I] - 500) / factor;
      F.printf("%c%.1f\n", XYZE[I], f);  //保存新位置到文件
    }
    F.close();  //关闭文件
    return Command("? /H.txt");
  }
  if (t.equalsIgnoreCase("H")) {    //HOME 让XYZBTE六个电机到指定位置
    if (SPIFFS.exists("/H.txt")) {  //如果有 H.txt 则执行
      Command("R H.txt");
      return output();
    } else
      for (int I = 0; I < ss; I++) {  //无 H.txt 则执行originPos变量值
        Servo180(I, originPos[I]);
      }
    Servo180(-1, 0);
    return output();
  }
  if (t.equalsIgnoreCase("Test")) {  //执行一段底座左右转动的测试动作
    ret = Test();
    return ret;
  }
  if (t.equalsIgnoreCase("S")) {  //保存XYZE电机新角度指令到自动执行文件
    ret = "";
    if (S.toInt() > 0) {                   //S xx 转为 delay xx 保存到文件
      ret = "delay " + String(S.toInt());  //保存delay xx到文件
    } else if (S != "") {                  //S delay xx 保存 delay xx  到文件
      ret = S;                             //S Y1500;S R test.txt;直接存到文件
    } else
      for (int I = 0; I < ss; I++) {  //判断位置发生变化的所有舵机并保存
        if (dms[I] != newPos[I]) {    //判断位置与上次保存后有无发生变化
          if (ret != "") ret += ";";  //如果有多个舵机有转动过,添加 ; 分隔符
          float f = (float)(newPos[I] - 500) / factor;
          ret += XYZE[I] + String(f, 1);  //保存新位置到文件
          dms[I] = newPos[I];             //保存新的X位置下次判断用
        }
      }
    if (ret != "") {                    //有 待追加命令
      File F = SPIFFS.open(file, "r");  //"r"=读取内容
      F.seek(0);                        //到首位置
      Auto = F.readString();            //读入文本
      Auto.trim();                      //删首尾空格与换行
      F.close();                        //关闭文件

      F = SPIFFS.open(file, "w");  //"w"=重写文件所有内容
      F.println(Auto);             //先保存原指令
      F.println(ret);              //再追加新指令
      F.flush();
      F.close();  //结束关闭文件
    }
    return ret;  //返回信息串口输出新指令
  }
  if (t.equalsIgnoreCase("D")) {      //D 1 删除最后一行;  D 3 删除多行动作指令
    File F = SPIFFS.open(file, "r");  //"r"=只读内容,"w"=重写内容
    F.seek(0);                        //到首位置
    Auto = F.readString();            //读入文本
    Auto.trim();                      //删首尾空格与换行
    F.close();                        //关闭文件

    v = S.toInt();  //D 3 删除末尾3行
    if (v < 1) v = 1;
    for (; v > 0; v--) {           //循环次数是要删除几行
      i = Auto.lastIndexOf("\n");  //倒找换行符
      if (i < 0) i = 0;            //-1=无换行符(改0,删除所有内容)
      Auto.remove(i);              //删除i后面的文本
    }

    F = SPIFFS.open(file, "w");  //"w"=重写文件所有内容
    F.seek(0);                   //到首位置
    F.print(Auto);               //保存剩余指令到文件
    F.close();                   //关闭文件
    return Auto;                 //输出删行后的剩余 动作指令
  }
  if (t.equalsIgnoreCase("C")) {                //C Auto;C test;清空指定文件内容并删除文件
    if (S == "") S = file;                      //参数若未指令文件，删除file
    if (!S.startsWith("/")) S = "/" + S;        //比较字符串前缀
    if (1 > S.indexOf(".", 1)) S = S + ".txt";  //比较字符串后缀
    SPIFFS.remove(S);                           //删除文件
    dms[6] = { 0 };
    return S;  //输出删除的文件
  }
  if (t.equalsIgnoreCase("step")) {  //改变舵机速度,step 0=原速,step 1=减速;
    Step = 0 != S.toInt();
    return "Step:" + String(Step);  //
  }
  if (t.equalsIgnoreCase("format")) {  //格式化 开发板闪存文件系统 清除所有文件,需要重新上传
    SPIFFS.format();                   //格式化
    FSInfo info;                       //信息
    SPIFFS.info(info);                 //取闪存文件系统信息
    ret = "闪存.已用:" + String(info.usedBytes) + "字节;可用:" + String(info.totalBytes) + "字节;";
    return ret;
  }
  if (t.equalsIgnoreCase("UP")) {  //UP and down  你想写啥就写啥,别问我
    Servo180(-1, 0);
    ret = "UP..";
    return ret;
  }
  if (t.equalsIgnoreCase("DIR")) {  //枚举文件并输出
    FSInfo info;                    //信息
    SPIFFS.info(info);              //取闪存文件系统信息
    ret = "闪存.已用:" + String(info.usedBytes) + "字节;可用:" + String(info.totalBytes) + "字节;\n";

    Dir dir = SPIFFS.openDir("/");  //打开根目录,枚举文件名
    while (dir.next()) {            //循环所有对象
      ret += dir.fileName();
      File F = dir.openFile("r");
      ret += "\t";              //Tab 制表符
      ret += String(F.size());  //取文件长度
      ret += "\n";
      F.close();
    }
    return ret;  //返回输出所有枚举到的文件信息
  }

  if (t.equalsIgnoreCase("R")) {     //R   运行file变量指定文件的动作指令
    v = S.toInt();                   //R 3 执行 Auto.txt 3次
    if (v != 0 && S == String(v)) {  //R Auto   执行Auto.txt 1次
      Autorun = v;                   //R test   执行test.txt 1次
      return ret;                    //若 R 参数为整数,设置 Autorun 变量循环执行 Auto.txt 次数
    }
    if (S == "") S = file;  //参数若未指定文件,则执行file变量里的文件
    Auto = S;
  } else Auto = t;                                     //对于不可识别的指令尝试以动作文件执行,如 "Q" 变会成 "/Q.txt" 若存在则执行
  if (!Auto.startsWith("/")) Auto = "/" + Auto;        //比较字符串前缀
  if (1 > Auto.indexOf(".", 1)) Auto = Auto + ".txt";  //比较字符串后缀
  if (SPIFFS.exists(Auto)) {                           //判断动作文件是否存在.若有此文件则执行
    ret = "R " + Auto + "\n";
    if (Response && Auto != "/H.txt") {  //R指令判断是否有待应答网页请求
      Response = false;                  //标记为已应答
      Web.send(200, "text/plain", ret);  //立即应答,避免长时间挂起网页
    }

    v = S.toInt();                           //Auto.txt 5 提取执行次数
    if (v < 1 || v > 256 || S != String(v))  //
      v = 1;                                 //不符合要求的次数重定为1次
    S = Auto;                                //另存文件名称
    for (; v > 0; v--) {
      File F = SPIFFS.open(S, "r");  //打开文件 "r"=只读取文件内容
      //F.seek(0);                     //到文件首位置
      Auto = F.readString();  //读内容到变量
      F.close();              //关闭文件

      //拆分多行命令 或以 ; 为分隔的多个命令
      int Run = Autorun;  //要运行的次数,当值有改变时停止
      i = 0;
      j = 0;  //i起始,j=;,v=\n
      do {
        i = Auto.indexOf(";", 0);   //查找分隔符 ;
        j = Auto.indexOf("\n", 0);  //查找换行符 \n
        if (i == -1 && j == -1) {   //没找到符号
          Command(Auto);
          break;                          //单个指令执行跳出
        } else if (i != 0 && j == -1) {   //单行有 ; 分隔的多个指令
          Command(Auto.substring(0, i));  //提取一个指令执行
          Auto.remove(0, i + 1);          //删除已执行的指令
        } else if (i == -1 && j != 0) {   //多行的 无分隔的单个指令
          Command(Auto.substring(0, j));  //提取单行指令执行
          Auto.remove(0, j + 1);          //删除已执行的此行
          Servo180(-1, 0);
        } else if (i < j) {               // ; 在 \n 之前
          Command(Auto.substring(0, i));  //提取一个指令执行
          Auto.remove(0, i + 1);          //删除已执行指令
        } else if (i > j) {               // \n 在 ; 之前
          Command(Auto.substring(0, j));  //提取一行指令执行
          Auto.remove(0, j + 1);          //删除已执行指令
          Servo180(-1, 0);
        } else break;
      } while (Auto.length() > 0 && Run == Autorun);  //-1=没找到换行 或 运行次数发生变化,结束循环.
      Servo180(-1, 0);
    }
    //Serial.println("R end");
    return ret + output();
  }


  //----------解析XYZE电机指令  底座X 前后Y 上下Z 夹子E
  t.toUpperCase();                 //指令转为大写
  for (i = 0; i < ss; i++) {       //循环所有舵机
    if (XYZE[i] == t.charAt(0)) {  //判断指令第1个字符为哪个电机
      if (t.length() == 1) {       //X --;  X ++;  X 1500;
        if (S == "--") {
          v = newPos[i] - 1;  //新角度=旧角度-1度(约11微秒)
        } else if (S == "++") {
          v = newPos[i] + 1;
        } else {
          v = newPos[i] + S.toInt();  //tofloat
        }
        Servo180(i, v);  //执行电机指令
        return "";
      }


      if (t.charAt(2) == '-') {         //X--    第3个字符
        v = newPos[i] - (int)factor;    //新角度=旧角度-1度(约11微秒)
      } else if (t.charAt(2) == '+') {  //X++    第3个字符
        v = newPos[i] + (int)factor;    //新角度=旧角度+1度(约11微秒)
      } else if (t.charAt(1) == '-') {  //X-?    第2个字符
        t = t.substring(2);             //提取第3个字符与后面的数字内容

        v = newPos[i] - (int)(t.toFloat() * factor);  //新角度=旧角度-N度(约11微秒)
      } else if (t.charAt(1) == '+') {                //X+?    第2个字符
        t = t.substring(2);                           //提取第3个字符与后面的数字内容
        float f = t.toFloat() * factor;
        if ((float)(f - (int)f) >= 0.45) f = f + 1.0;
        v = newPos[i] + (int)f;  //新角度=旧角度+N度(约11微秒)
      } else {                   //X??? 绝对定位
        t = t.substring(1);      //提取第2个字符与后面的数字内容
        v = todms(t.toFloat());  //新角度=数字内容值 或 新脉宽值
      }
      Servo180(i, v);  //执行电机指令
      return "";
    }
  }
  return ret;
}

//---------------------------------------------------------------------------
//收到 test 指令会执行这个测试程序,控制 X 和 E 舵机转动
//可供参考用,自已编程使用 变量存取 与 逻辑条件 做复杂功能
//---------------------------------------------------------------------------
String Test() {  //一个简单的测试底座来回转到的功能

  Command("step 1");  //设定为 减速
  Command("H");       //所有舵机复位
  for (int i = 1; i < 6; i++) {
    Command("X" + String(90 + i * 10));  //X底座转向一侧
    Command("E-20");                     //E开夹20度
    if (i == 4) Command("Y+10");         //当左右转第4次时Y前顷10度
    Command("X" + String(90 - i * 10));  //X底座转向另一侧
    Command("E+20");                     //E闭夹20度
  }
  Command("H");  //所有舵机复位
  return "Test()";
}


//---------------------------------------------------------------------------
//
//
//
//
//---------------接受机械臂网页控制指令函数 -------------------------------
void command() {  //网址里不能用 + 符号,会变成空格
  String message = "";
  Autorun = 0;                            //有任何命令时停止自动脚本
  if (Web.args() == 0) {                  //http://ip/command 无指令时返回所有舵机状态
    if (WiFi.status() == WL_CONNECTED) {  //成功连网
      IPAddress IP = WiFi.localIP();
      message = IP.toString() + "\n";
    }
    message += output();
  }

  Response = true;     //标记需要应答
  static String date;  //静态变量时间戳

  for (int i = 0; i < Web.args(); i++) {  // http://ip/command?x90&y90
    String t = Web.argName(i);            // 提取一个指令
    if (t.equalsIgnoreCase("time")) {
      if (date != Web.arg(i)) date = Web.arg(i);
      continue;
    }
    Serial.println(t);  // 网页脚本XMLHttpRequest发来GET请求把"+"号变成" "空格,而" "空格还是" "空格.

    t.replace("_", "+");  // 把"_"恢复为"+"; 网页脚本arg=arg.replace(/\+/g ,"_"); //把'+'替换为'_'
    
    Serial.print("HTTP指令:");
    Serial.println(t);
    message += "\n" + t + "\n";
    message += Command(t);  // 解析 并 执行 命令
  }
  if (0 < Web.args())
    if (Servo180(-1, 0))
      message += output();
  Web.send(200, "text/plain", message);
  Response = false;  //标记已应答
}
//------------------保存一些变量数据参数到配置文件------------------------
//
//
//-------------------------------------------------------------------
void Config() {  //保存一些变量中的值到配置文件/config.json

  if (Web.args() > 0) {
    float v = 1.0;
    DynamicJsonDocument doc(2048);          //堆内存JSON文档对象
    doc["AP_SSID"] = Web.arg("AP_SSID");    //控制板要创建的WIFI热点
    doc["AP_PSK"] = Web.arg("AP_PSK");      //控制板要创建的WIFI密码
    doc["STA_SSID"] = Web.arg("STA_SSID");  //控制板要连接的路由器热点
    doc["STA_PSK"] = Web.arg("STA_PSK");    //控制板要连接的路由器密码
    doc["ss"] = ss;

    doc["Servo"][0] = String(XYZE[0]);  //舵机编号
    doc["Servo"][1] = String(XYZE[1]);  //舵机编号
    doc["Servo"][2] = String(XYZE[2]);  //舵机编号
    doc["Servo"][3] = String(XYZE[3]);  //舵机编号
    doc["pin"][0] = pin[0];             //舵机GPIO
    doc["pin"][1] = pin[1];             //舵机GPIO
    doc["pin"][2] = pin[2];             //舵机GPIO
    doc["pin"][3] = pin[3];             //舵机GPIO

    originPos[0] = todms(Web.arg("Xraw").toFloat());
    v = (float)(originPos[0] - 500) / factor;
    doc["originPos"][0] = String(v, 1);  //输出带1位精度的角度值
    originPos[1] = todms(Web.arg("Yraw").toFloat());
    v = (float)(originPos[1] - 500) / factor;
    doc["originPos"][1] = String(v, 1);  //输出带1位精度的角度值
    originPos[2] = todms(Web.arg("Zraw").toFloat());
    v = (float)(originPos[2] - 500) / factor;
    doc["originPos"][2] = String(v, 1);  //输出带1位精度的角度值
    originPos[3] = todms(Web.arg("Eraw").toFloat());
    v = (float)(originPos[3] - 500) / factor;
    doc["originPos"][3] = String(v, 1);  //输出带1位精度的角度值

    minPos[0] = todms(Web.arg("Xmin").toFloat());
    v = (float)(minPos[0] - 500) / factor;
    doc["minPos"][0] = String(v, 1);  //输出带1位精度的角度值
    minPos[1] = todms(Web.arg("Ymin").toFloat());
    v = (float)(minPos[1] - 500) / factor;
    doc["minPos"][1] = String(v, 1);  //输出带1位精度的角度值
    minPos[2] = todms(Web.arg("Zmin").toFloat());
    v = (float)(minPos[2] - 500) / factor;
    doc["minPos"][2] = String(v, 1);  //输出带1位精度的角度值
    minPos[3] = todms(Web.arg("Emin").toFloat());
    v = (float)(minPos[3] - 500) / factor;
    doc["minPos"][3] = String(v, 1);  //输出带1位精度的角度值

    maxPos[0] = todms(Web.arg("Xmax").toFloat());
    v = (float)(maxPos[0] - 500) / factor;
    doc["maxPos"][0] = String(v, 1);  //输出带1位精度的角度值
    maxPos[1] = todms(Web.arg("Ymax").toFloat());
    v = (float)(maxPos[1] - 500) / factor;
    doc["maxPos"][1] = String(v, 1);  //输出带1位精度的角度值
    maxPos[2] = todms(Web.arg("Zmax").toFloat());
    v = (float)(maxPos[2] - 500) / factor;
    doc["maxPos"][2] = String(v, 1);  //输出带1位精度的角度值
    maxPos[3] = todms(Web.arg("Emax").toFloat());
    v = (float)(maxPos[3] - 500) / factor;
    doc["maxPos"][3] = String(v, 1);  //输出带1位精度的角度值
    doc["Autorun"] = Web.arg("Auto").toInt();  //板子通电自动运行Auto.txt次数
    doc["null"] = "null";
    Web.sendHeader("Location", "/index.html");
    Web.send(303);  //303跳转

    File F = SPIFFS.open("/config.json", "w");  //创建重写文件
    serializeJson(doc, F);                      //输出JSON格式内容到文件
    serializeJson(doc, Serial);                 //输出JSON格式内容到串口
    F.flush();
    F.close();  //关闭文件
    doc.clear();

    ESP.restart();  //软重启
  }
  //---------------------------------------------------------
  if (Web.args() == 0) {
    String html = "<!DOCTYPE html>";
    html += "<html lang='zh-CN'>";
    html += "<head>";
    html += "  <meta  charset='UTF-8'>";
    html += "  <title>乔帮主的IT视界</title>";
    html += "  <style type='text/css'>";
    html += "    .txt{";
    html += "      width :50px;";
    html += "      height:20px;";
    html += "    }";
    html += "  </style>";
    html += "</head>";
    html += "<body>";
    html += "  <center>";

    html += "   <h1>乔帮主的IT视界-机械臂固件配置</h1>";
    html += "   <p>警告:舵机不防堵,转不过去的角度需要最大最小限制,不然会发烫烧坏</P>";
    html += "   <form action='config' method='get'>";
    html += "    <p>";
    html += "    AP_SSID:<input type='text' name='AP_SSID' value='" + AP_SSID + "'>控制板要创建的 WIFI 热点名称<br>";
    html += "    &nbspAP_PSK:<input type='text' name='AP_PSK' value='" + AP_PSK + "'>控制板要创建的 WIFI 热点密码<br>";
    html += "    </p>";
    html += "    <p>";
    html += "    STA_SSID:<input type='text' name='STA_SSID' value='" + STA_SSID + "'>控制板要连接的路由器热点名称<br>";
    html += "    &nbspSTA_PSK:<input type='text' name='STA_PSK' value='" + STA_PSK + "'>路由器热点密码,删除可跳过连接<br>";
    html += "    </p>";

    html += "    <p>";
    html += "    舵机编号:<input type='text' class='txt'  readonly='readonly' value='X'>";
    html += "            <input type='text' class='txt'  readonly='readonly' value='Y'>";
    html += "            <input type='text' class='txt'  readonly='readonly' value='Z'>";
    html += "            <input type='text' class='txt'  readonly='readonly' value='E'>";

    html += "    &nbsp各舵机&nbsp&nbsp编号名称</br>";
    html += "    初始角度:<input type='text' class='txt' name='Xraw' value='" + String((float)(originPos[0] - 500) / factor, 1) + "'>";
    html += "            <input type='text' class='txt' name='Yraw' value='" + String((float)(originPos[1] - 500) / factor, 1) + "'>";
    html += "            <input type='text' class='txt' name='Zraw' value='" + String((float)(originPos[2] - 500) / factor, 1) + "'>";
    html += "            <input type='text' class='txt' name='Eraw' value='" + String((float)(originPos[3] - 500) / factor, 1) + "'>";

    html += "    舵机通电初始角度<br>";

    html += "    最小角度:<input type='text' class='txt' name='Xmin' value='" + String((float)(minPos[0] - 500) / factor, 1) + "'>";
    html += "            <input type='text' class='txt' name='Ymin' value='" + String((float)(minPos[1] - 500) / factor, 1) + "'>";
    html += "            <input type='text' class='txt' name='Zmin' value='" + String((float)(minPos[2] - 500) / factor, 1) + "'>";
    html += "            <input type='text' class='txt' name='Emin' value='" + String((float)(minPos[3] - 500) / factor, 1) + "'>";

    html += "    舵机最小角度限制<br>";
    html += "    最大角度:<input type='text' class='txt' name='Xmax' value='" + String((float)(maxPos[0] - 500) / factor, 1) + "'>";
    html += "            <input type='text' class='txt' name='Ymax' value='" + String((float)(maxPos[1] - 500) / factor, 1) + "'>";
    html += "            <input type='text' class='txt' name='Zmax' value='" + String((float)(maxPos[2] - 500) / factor, 1) + "'>";
    html += "            <input type='text' class='txt' name='Emax' value='" + String((float)(maxPos[3] - 500) / factor, 1) + "'>";

    html += "   舵机最大角度限制<br>";
    html += "    </p>";

    html += "    Auto次数:<input type='text' style='width:100px;height:20px;' name='Auto' value='0'>通电就执行 Auto.txt 多少次,-1 为无限次<br>";
    html += "    <p><a href='/index.html'  >[返回首页]</a>&nbsp&nbsp";
    html += "       <input type='submit' value='提交设置'>&nbsp提交后会重启控制板,要重连WIFI,重开控制网页";
    html += "    </p>";

    html += "   </form>";

    html += "</center>";
    html += "</body>";
    html += "</html>";
    Web.send(200, "text/html", html);
  }
}

//--------------------------------处理文件上传函数-------------------------------------------
void handleFileUpload() {
  static File F;  //静态变量 文件对象用于文件上传

  HTTPUpload& UP = Web.upload();         //上传对象
  if (UP.status == UPLOAD_FILE_START) {  // 如果 状态=UPLOAD_FILE_START 开始上传
    String filename = UP.filename;       // 上传的文件名
    if (!filename.startsWith("/"))
      filename = "/" + filename;     // 为文件名前加上"/"
    F = SPIFFS.open(filename, "w");  // 在SPIFFS中建立W重写文件用于写入用户上传的文件数据
  }
  if (UP.status == UPLOAD_FILE_WRITE) {  // 如果上传状态为UPLOAD_FILE_WRITE
    if (F)
      F.write(UP.buf, UP.currentSize);  // 向SPIFFS文件写入浏览器发来的文件数据 2048
  }
  if (UP.status == UPLOAD_FILE_END) {  // 如果上传状态为UPLOAD_FILE_END
    F.close();                         //关闭文件,完成上传
  }
  if (UP.status == UPLOAD_FILE_ABORTED) {  //中断或取消上传
    F.close();
  }
}

void FileUpload_OK() {

  HTTPUpload& UP = Web.upload();
  String html = "<!DOCTYPE html>";
  html += "<html lang='zh-CN'>";
  html += "<head>";
  html += "  <meta  charset='UTF-8'>";
  html += "  <title>乔帮主的IT视界</title>";
  html += "</head>";
  html += "<body>";
  html += "  <center>";
  html += "    <h1>上传文件成功.Size: " + String(UP.contentLength) + "</h1>";  //文件成功保存
  html += "    <a href='/index.html'>[首页]</a>";
  html += "    <a href='/upload.html'>[继续上传]</a>";
  html += "  </center>";
  html += "</body>";
  html += "</html>";
  Web.send(200, "text/html", html);
}
//-------------------------------------------------------
//
//
//---------------处理用户浏览器的HTTP访问-------------------
void handleUserRequest() {
  String url = Web.uri();  //获取用户请求网址信息

  Serial.print(" 用户请求 ");
  Serial.println(url);

  if (url.endsWith("/")) {      //如果访问地址以"/"为结尾
    url = "/index.html";        //将访问地址修改为/index.html文件
    if (!SPIFFS.exists(url)) {  //还未上传首页文件跳转 文件上传 页
      Web.sendHeader("Location", "/upload.html");
      Web.send(303);  //303跳转
      return;
    }
  }

  if (url == "/upload.html") {  //请求上传文件 http://IP/upload.html
    String html = "<!DOCTYPE html>";
    html += "<html lang='zh-CN'>";
    html += "<head>";
    html += "  <meta  charset='UTF-8'>";
    html += "  <title>乔帮主的IT视界</title>";
    html += "</head>";
    html += "<body>";
    html += "  <center>";
    html += "    <h1>ESP8266 SPIFFS 文件上传</h1>";
    html += "    <form method='POST' enctype='multipart/form-data'>";
    html += "      <input type='file' name='data' multiple>";
    html += "      <input class='button' type='submit' value='上传'>";
    html += "    </form>";
    html += "  </center>";
    html += "</body>";
    html += "</html>";
    Web.send(200, "text/html", html);
    return;
  }

  if (SPIFFS.exists(url)) {              // 测试文件是否存在
    File file = SPIFFS.open(url, "r");   // "r"=只读打开文件
    Web.streamFile(file, getType(url));  // 发送文件给浏览器
    file.close();                        // 关闭文件
  } else {
    Web.send(404, "text/plain", "404 Not Found");  //如果没有找到文件,则回复404 Not Found
  }
}
//---------------------------------------------------------------------------
String getType(String filename) {  // 获取文件类型
  if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  else if (filename.endsWith(".txt")) return "text/plain;charset=UTF-8";
  else if (filename.endsWith(".json")) return "text/plain;charset=UTF-8";
  return "text/plain";
}
