// arduinoimg/01.BLink_LED.png         //接线图示文件

const int LED=13;           //定义 LED 引脚为 13

void setup()
{
  pinMode(LED,OUTPUT);      // 定义 13 为输出引脚
}

void loop()
{
  digitalWrite(LED,HIGH);   // 设定 LED 为高电平，即点亮
  delay(1000);              // 持续 1 秒
  digitalWrite(LED,LOW);    // 设定 LED 为低电平，即熄灭
  delay(1000);              // 持续 1 秒

}
