#include <Arduino.h>
#include <ArduinoJson.h>             //使用Json文件格式做配置文件
#include <Servo.h>                   //引用 舵机 功能库头文件
Servo S;                              // 创建四个舵机对象
int lastPos = 90;                     // 舵机上次转动到的角度
int newPos = 90;                      // 舵机新的目标角度
int maxPos = 180;
int minPos =0;
float factor = 11.11; // 每转一度对应的脉冲宽度，11.11=(2500-500)/180度

/*------------------------------
// 把 0-180 角度值转为对应高电平脉冲信号时间宽度（脉冲宽度）
输入：degree， 目标角度值
返回值：输入角度所对应的脉冲宽度，单位是微秒。大于 360时，直接返回，或 500 ， 2500。
--------------------------------------*/
int toPwm(float degree)
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

void go(int Value){
  newPos = constrain(toPwm(Value), minPos, maxPos); // 指定新的目标角度

  S.write(newPos);
}

void setup()
{
  Serial.begin(115200);           // 开启串口通信 波特率115200,串口监视器也要相同波特率,不然会乱码
  pinMode(LED_BUILTIN, OUTPUT);   // 初始化8266开发板LED信号灯的GPIO口为输出.
  digitalWrite(LED_BUILTIN, LOW); // Mini ESP8266板LED_BUILTIN=GPIO 2,LOW=亮灯,HIGH=灭灯

  S.attach(D2, 500, 2500); // 绑定针脚,设置信号脉冲宽度范围, 以便精确控制转到的角度;
  minPos = toPwm((float)minPos);
  maxPos = toPwm((float)maxPos);
  S.write(toPwm(90)); // 写入新角度值,控制舵机转动
  delay(500);         // 等待该舵机转到目标角度.

  digitalWrite(LED_BUILTIN, LOW); // 亮灯
}

void loop()
{
  //---------------只监听电脑串口输入指令-------------------
  if (Serial.available() > 0)
  {                                 // 有串口数据>0字节
    String t = Serial.readString(); //?SDCHRA XYZBTE
    t.trim();                       // 删首尾空格与换行
    Serial.println(t);              // 串口回显指令
    go(t.toFloat());
  }
}
