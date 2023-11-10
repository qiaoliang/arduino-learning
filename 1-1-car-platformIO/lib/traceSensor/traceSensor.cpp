#include <Arduino.h>
#define TRACE_PIN1 A1  // 寻迹引脚， 车右起第1个
#define TRACE_PIN2 A2  // 寻迹引脚右2
#define TRACE_PIN3 A3  // 寻迹引脚右3
#define TRACE_PIN4 A4  // 寻迹引脚右4

void TraceSensor_Init() {
  pinMode(TRACE_PIN1, INPUT);
  pinMode(TRACE_PIN2, INPUT);
  pinMode(TRACE_PIN3, INPUT);
  pinMode(TRACE_PIN4, INPUT);
}

unsigned char trace_check() {
    unsigned char t1, t2, t3, t4 = 0x00;
    t4 = digitalRead(TRACE_PIN4);
    t3 = digitalRead(TRACE_PIN3);
    t2 = digitalRead(TRACE_PIN2);
    t1 = digitalRead(TRACE_PIN1);
    unsigned char ret = 0x00;
    ret |=t4<<3;
    ret |= t3 <<2;
    ret |= t2 <<1;
    ret |= t1;
    return ret;
}

unsigned char last_trace;
void Trace_DebugInfo(){
  unsigned char trace = trace_check();
  if(last_trace!=trace){
    last_trace = trace;
    Serial.print("寻迹数据: ");
    Serial.println(last_trace);
  }
}