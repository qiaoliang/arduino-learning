

#define TRACE_PIN1 A0  // 寻迹引脚， 车右起第1个
#define TRACE_PIN2 A1  // 寻迹引脚右2
#define TRACE_PIN3 A2  // 寻迹引脚右3
#define TRACE_PIN4 A3  // 寻迹引脚右4



void printBinary(unsigned char num) {
  for (int i = 7; i >= 0; i--) {
    Serial.print(bitRead(num, i));
  }
  Serial.println();
}
extern Rover* rover;  // 在主文件中已经声明并初始化了

void traceSensor_Init() {
  pinMode(TRACE_PIN1, INPUT);
  pinMode(TRACE_PIN2, INPUT);
  pinMode(TRACE_PIN3, INPUT);
  pinMode(TRACE_PIN4, INPUT);
}

void trace_check() {
    unsigned int t1, t2, t3, t4 = 0x00;
    t4 = digitalRead(TRACE_PIN4);
    t3 = digitalRead(TRACE_PIN3);
    t2 = digitalRead(TRACE_PIN2);
    t1 = digitalRead(TRACE_PIN1);
    char* trace_signal = (char*)malloc(4 * sizeof(char));
    // 0 亮灯，不在黑线上。1 是灭灯，在黑线上方。
    sprintf(trace_signal, "%d%d%d%d", t4, t3, t2, t1);
    Serial.println(trace_signal);
    if (t4==0 && t3 == 1 && t2 == 1 && t1==0) {
      rover->forward();
      Serial.println("forward");
    } else if ((t4==1 && t3 == 1 && t2 == 0 && t1==0) || (t4==1 && t3 == 1 && t2 == 1 && t1==0)) {
      rover->right();
      Serial.println("right");
    } else if ((t4==0 && t3 == 0 && t2 == 1 && t1==1) || (t4==0 && t3 == 0 && t2 == 0 && t1==1)|| (t4==0 && t3 == 1 && t2 == 1 && t1==1)) {
      rover->left();
      Serial.println("left");
    } else if ((t4==1 && t3 == 1 && t2 == 1 && t1==1)) {
      Serial.println("stop");
      rover->stop();
    }
}