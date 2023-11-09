#include <IRremote.hpp>

// 车上红外摇控接收器的引脚
#define IR_RECEIVE_PIN A5

// 红色发射器键盘的定义
#define KEY_2 0xE718FF00
#define KEY_5 0xE31CFF00
#define KEY_8 0xAD52FF00
#define KEY_4 0xF708FF00
#define KEY_6 0xA55AFF00

long last_irdata;
void IR_Init(){
  IrReceiver.begin(IR_RECEIVE_PIN);
}
long ir_item;
long readIr(){
  long ret = 0;
  if (IrReceiver.decode()) {
    ret = IrReceiver.decodedIRData.decodedRawData;             // 得到接收到的按键值                     
    IrReceiver.resume();                                           // Enable receiving of the next value
  } 
  return ret;
}
long IR_detect(){
  long ir_item = readIr();
  switch(ir_item){
    case KEY_2:
      return 2;
    case KEY_5:
      return 5;
    case KEY_8:
      return 8;
    case KEY_4:
      return 4;
    case KEY_6:
      return 6;
    default:
      return 0xFF;
  }
}
void IR_DebugInfo(){
  long value =IR_detect();
  if(value!=0xFF){
    Serial.print("红外遥控：");
    Serial.println(value, HEX);   //用16进制显示 
  }
}
