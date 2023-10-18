#define TRACE_PIN1  A0  // 寻迹引脚右1
#define TRACE_PIN2  A1  // 寻迹引脚右2
#define TRACE_PIN3  A2  // 寻迹引脚右3
#define TRACE_PIN4  A3  // 寻迹引脚右4

void printBinary(unsigned char num) {
  for (int i = 7; i >= 0; i--) {
    Serial.print(bitRead(num, i));
  }
  Serial.println();
}
extern Rover* rover;          // 在主文件中已经声明并初始化了

uint8_t trace_signal,lastTrace=0;
void trace_check(){
  lastTrace = trace_signal;
  trace_signal = digitalRead(TRACE_PIN4);
  trace_signal = (trace_signal<<1)|digitalRead(TRACE_PIN3);
  trace_signal = (trace_signal<<1)|digitalRead(TRACE_PIN2);
  trace_signal = (trace_signal<<1)|digitalRead(TRACE_PIN1);
  if(lastTrace!= trace_signal) printBinary(trace_signal);
  if (trace_signal == 0b0000) {
    rover->stop();
  }
  if (trace_signal == 0b1100) {
    rover->right();
  }
  if (trace_signal == 0b0011) {
    rover->left();
  }
  if (trace_signal == 0b0110) {
    rover->act();
  }
}