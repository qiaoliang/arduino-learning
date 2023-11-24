
#include <Arduino.h>
#include "FS.h"
#include <Ticker.h>  //  Ticker是Arduino Core For ESP8266 内置的一个定时器库，这个库用于规定时间后调用函数。
//  不建议使用Ticker回调函数来阻塞IO操作（网络、串口、文件）；可以在Ticker回调函数中设置一个标记，在loop函数中检测这个标记；
//  对于arg，必须是 char, short, int, float, void *, char *之一

#include "WiFi.h"
#include "myServo.h"

Ticker flipper;

int count_in_second = 0;
int conunt_in_microsecond = 0;
void flip_in_second()
{
  int state = digitalRead(LED_BUILTIN); // get the current state of GPIO1 pin
  digitalWrite(LED_BUILTIN, !state);    // set pin to the opposite state

  ++count_in_second;
  // 当翻转次数达到20次的时候，切换led的闪烁频率，每隔0.1s翻转一次
  if (count_in_second == 20)
  {
    flipper.attach(0.1, flip_in_second); // 每隔xx秒周期性执行, 0.1 为间隔时间,单位是秒; flip 是要执行的函数,在前面定义了.
  }
  // 当次数达到120次的时候关闭ticker
  else if (count_in_second == 120)
  {
    flipper.detach(); // 必须 Detach 掉，否则会一直执行
  }
}

void setup()
{
  Serial.begin(115200);

  while (!Serial)
    ;
  MyServo_Init();
  WiFi_Init();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // 每隔0.3s 翻转一下led状态
  flipper.attach(0.3, flip_in_second);
}

String  ExcuteCmd(String cmd){
    // TODO: 执行命令
    return cmd;
}
void loop()
{
  Web.handleClient(); // 处理客户HTTP访问,上传文件,更新固件

  //---------------监听电脑串口输入指令-------------------
  if (Serial.available() > 0)
  {                                 // 有串口数据>0字节
    String t = Serial.readString(); //?SDCHRA XYZBTE
    t.trim();                       // 删首尾空格与换行
    Serial.println(t);
    Serial.println(ExcuteCmd(t));
  }
}