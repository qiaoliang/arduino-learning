#include <Arduino.h>
#include <ESP8266WiFi.h>             //编译此代码需要先安装ESP8266开发板文件包,并且只能上传到ESP8266芯片的开发板才能运行.
#include <ESP8266WebServer.h>        //小型HTTP网页服务
#include <ESP8266HTTPUpdateServer.h> //寄生网页服务,接受 固件.bin 或 系统.bin http://X.X.X.X/upbin Firmware:固件,FileSystem:文件系统
#include <ArduinoJson.h>             //使用Json文件格式做配置文件
#include "FS.h"                      //ESP8266开发板自带4MB闪存空间,可以用来读写存删文件
#include <Servo.h>                   //引用 舵机 功能库头文件

String AP_SSID = "ESP8266"; // ESP8266自已创建的热点名称
String AP_PSK = "12345678"; // ESP8266自已Wifi热点的密码

String STA_SSID = "MYHOME";  // 这里写你家路由器WIFI或手机电脑的移动热点名称
String STA_PSK = "12345678"; // 这里写你家WIFI或手机热点密码

bool Response = false;           // 应答标记 true false.网页请求应该快速回答,但R指令执行动作文件可能要很久.
ESP8266WebServer Web(80);        // 建立Web服务对象,HTTP端口80
ESP8266HTTPUpdateServer Updater; // ESP8266 网络[更新固件]服务

const int ss = 4;                     // 4轴
Servo S;                              // 创建四个舵机对象
char XYZE[ss] = {'X', 'Y', 'Z', 'E'}; // 定义4个电机从底座到夹子为 XYZBE
int pin[ss] = {D2, D3, D0, D8};       // 开发板的数字针脚用来接4个电机
int originPos = 90;                   // 舵机的初始角度
int lastPos = 90;                     // 舵机上次转动到的角度
int newPos = 90;                      // 舵机新的目标角度
int minPos = 0;                       // 舵机可转动的最小角度
int maxPos = 180;                     // 舵机可转动的最大角度

bool Step = true;     // true=减速(按 1 度角减速),false=0 舵机原速转动
float factor = 11.11; // 每转一度对应的脉冲宽度，11.11=(2500-500)/180度

String Cmd="";
String Cmdret="";

    /*------------------------------
    // 把 0-180 角度值转为对应高电平脉冲信号时间宽度（脉冲宽度）
    输入：degree， 目标角度值
    返回值：输入角度所对应的脉冲宽度，单位是微秒。大于 360时，直接返回，或 500 ， 2500。
    --------------------------------------*/
    int
    toPwm(float degree)
{ // 返回  把角度值转换为维持高电平的时间长度
  unsigned long pulseWidth;
  if (degree < 360.0)
  { // 值少于360视为角度,否则，入参被视为时间长度，单位为微秒
    unsigned long pulseWidth = degree * factor;
    if ((float)(pulseWidth - (int)pulseWidth) >= 0.45) // 四舍五入
      degree = degree + 1.0;
    return 500 + (int)pulseWidth; // 500 对应着起点，即 0 度。
  }
  else
  {
    pulseWidth = constrain((int)degree, 500, 2500); // 取值500~2500间,9克舵机都对应这个数值。
    return pulseWidth;
  }
}

bool ServoKeepRunning()
{
  unsigned long ms = millis(); // 记录板子启动到现在的时间
  int  maxms = 0;

  while (newPos != lastPos)
  {                                    // 新旧值不同,需要发信号转动
    int range = abs(newPos - lastPos); // 计算当前转到目标的脉宽差
    maxms = max(maxms, range);         // 支持多舵机同时转动,记下最长的耗时。
    if (range > (int)factor)
    { // 需要减速，并且 range 大于 1 度对应的时长，
      if (newPos > lastPos)
      {
        lastPos += (int)factor; // 正转1度
      }
      else
      {
        lastPos -= (int)factor; // 反转1度

        S.write(lastPos); // 向对应的引脚发送信号
        delay(1);
      }
    }
  }

  // 9克舵机的空载速度是每2000微秒可以转1度 ，而 2000微秒相当于转11.11 个脉冲宽度(因为每转1个脉宽信号，需要消耗182微秒)
  // 计算需要转动时间最长的舵机是否在当前时间里完成了转动。
  // 如果没有完成，则等待一段时间
  ms = maxms / (int)factor * 2 + ms;
  if (ms > millis())
  { // 检查目前已消耗的时间与转到目标角度需要的时间
    delay(constrain(ms - millis(), 0, 400));
  }

  return maxms != 0; // 如果maxms 没有改变，仍旧是0，返回 flase，说明所有舵机都不需要转动。否则，说明有发送过舵机转动指令信号，返回 true
}

void Servo180(int Value)
{ // 脉宽高电平 500微秒 到 2500微秒 之间，对应舵机0°～180°可转角度
  if (newPos != lastPos)
  {                     // 如果上次动作还没有完成
    ServoKeepRunning(); // 则立即去执行完
  }
  newPos = constrain(toPwm(Value), minPos, maxPos); // 指定新的目标角度
}

String Command(String targetDegree)
{
  int v = toPwm(targetDegree.toFloat()); // 新角度=数字内容值 或 新脉宽值
  Servo180(v);                           // 执行电机指令
  return "";
}
//-------------------------------------------------------------
String output()
{                              // 返回Json格式的所有舵机当前角度信息
  StaticJsonDocument<256> doc; // 创建一个栈内存JSON文档对象


    String s = "X"; // 舵机编号
    float v = (float)(newPos - 500.0) / factor;
    doc[s] = String(v, 1); // 输出带1位精度的角度值

  String ret;
  serializeJson(doc, ret); // 单行格式 到变量
  doc.clear();
  return ret; //{"X":90,"Y":90,"Z":90,"E":90,"B":90,"T":90}
}
String splitAndExecuteCmd(String t)
{ // 拆分多行命令 或以 ; 为分隔的多个命令
  String ret = "";
  long i[2] = {0}; //[0]=; [1]=\n
  do
  {
    i[0] = t.indexOf(";", 0);  // 查找分隔符 ;
    i[1] = t.indexOf("\n", 0); // 查找换行符 \n
    if (i[0] == -1 && i[1] == -1)
    { // 没找到符号
      ret = Command(t);
      break; // 单个指令执行跳出
    }
    else if (i[0] != 0 && i[1] == -1)
    {                                      // 单行有 ; 分隔的多个指令
      ret = Command(t.substring(0, i[0])); // 提取一个指令执行
      t.remove(0, i[0] + 1);               // 删除已执行的指令
    }
    else if (i[0] < i[1])
    {                                      // ; 在 \n 之前
      ret = Command(t.substring(0, i[0])); // 提取一个指令执行
      t.remove(0, i[0] + 1);               // 删除已执行指令
    }
    else if (i[0] > i[1])
    {                                      // \n 在 ; 之前
      ret = Command(t.substring(0, i[1])); // 提取一行指令执行
      t.remove(0, i[1] + 1);               // 删除已执行指令
      if (ServoKeepRunning())
        Serial.println(output());
    }
    else
      break;
  } while (t.length() > 0); // 未查找完 继续循环
  if (ret == "")
    if (ServoKeepRunning())
      ret = output();
  return ret;
}
void setup()
{
  Serial.begin(115200);           // 开启串口通信 波特率115200,串口监视器也要相同波特率,不然会乱码
  pinMode(LED_BUILTIN, OUTPUT);   // 初始化8266开发板LED信号灯的GPIO口为输出.
  digitalWrite(LED_BUILTIN, LOW); // Mini ESP8266板LED_BUILTIN=GPIO 2,LOW=亮灯,HIGH=灭灯

  S.attach(D2, 500, 2500); // 绑定针脚,设置信号脉冲宽度范围//S.detach();
  originPos = toPwm((float)originPos);
  lastPos = originPos; // originPos
  newPos = originPos;  // originPos
  minPos = toPwm((float)minPos);
  maxPos = toPwm((float)maxPos);
  S.write(originPos); // 写入新角度值,控制舵机转动
  delay(500);         // 等待该舵机转到目标角度.

  digitalWrite(LED_BUILTIN, LOW); // 亮灯

}

void loop()
{

  if (Cmd != "")
  {                                   // 如果cmd全局变量里有未执行命令,现在执行
    Cmdret = splitAndExecuteCmd(Cmd); // 分析指令并调用Command执行
    Cmd = "";
  }

  //---------------监听电脑串口输入指令-------------------
  if (Serial.available() > 0)
  {                                 // 有串口数据>0字节
    String t = Serial.readString(); //?SDCHRA XYZBTE
    t.trim();                       // 删首尾空格与换行
    Serial.println(t);              // 串口回显指令
    Serial.println(splitAndExecuteCmd(t));
  }
}
