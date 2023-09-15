int ledArray[]={8,9,10,11};         // LED 数组
int count = 0;
int timer =75;

void setup()
{
  for(count =0;count <4;count ++)       // 设置 4 个 LED 的引脚输出
  {
    pinMode(ledArray[count],OUTPUT);
  }
}

void loop()
{
  for(count =0;count <4;count ++) 
  {
    digitalWrite(ledArray[count],HIGH);
    delay(1000);
    digitalWrite(ledArray[count],LOW);
    delay(1000);
  }
}
