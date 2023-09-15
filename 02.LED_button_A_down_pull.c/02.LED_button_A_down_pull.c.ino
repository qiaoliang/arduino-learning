// https://baijiahao.baidu.com/s?id=1669929447257804538&wfr=spider&for=pc

// 按住就亮，松开就灭

const int LED = 13;
const int BUTTON =2;

int pre_state = 0;
void setup()
{
  pinMode(LED,OUTPUT);
  pinMode(BUTTON,INPUT);
  digitalWrite(LED, LOW);
  pre_state = 1;
}

void loop()
{
  int curr_state = digitalRead(BUTTON);
  if(curr_state != pre_state){//读取 2 号引脚状态，并做判断
    digitalWrite(LED, curr_state);
    pre_state = curr_state;
  }
}
