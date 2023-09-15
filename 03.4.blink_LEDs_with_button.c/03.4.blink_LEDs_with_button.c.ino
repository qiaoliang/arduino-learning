volatile int state = LOW;
int ledArray[]={8,9,10,11};         // LED 数组
int count = 0;
int timer =75;
int pause = 500;

void setup()
{
  for(count =0;count <4;count ++)       // 设置 4 个 LED 的引脚输出
  {
    pinMode(ledArray[count],OUTPUT);
  }
  attachInterrupt(0,ledOnOff,RISING);   // 初始化中断
}

void loop()
{  
  if (state)
  {
    for(count =0;count <4;count ++)       // 
    {
      digitalWrite(ledArray[count],HIGH);
      delay(timer);
    }
    delay(pause);
  }
}

void ledOnOff()
{
  static unsigned long lastMillis = 0;
  unsigned long newMillis = millis();
  if(newMillis - lastMillis < 50)         // 进行button的消抖处理
  {
  }else{
    state = !state;
    lastMillis = newMillis;
  }
}
