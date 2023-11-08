#include <IRremote.hpp>

// 车上红外摇控接收器的引脚
#define IR_RECEIVE_PIN A0

// 红色发射器键盘的定义
#define KEY_2 0xE718FF00
#define KEY_5 0xE31CFF00
#define KEY_8 0xAD52FF00
#define KEY_4 0xF708FF00
#define KEY_6 0xA55AFF00

void IR_Init(){
  IrReceiver.begin(IR_RECEIVE_PIN);
}
long ir_item;

long IR_detect(){
  if (IrReceiver.decode()) {
    ir_item = IrReceiver.decodedIRData.decodedRawData;             // 得到接收到的按键值
    Serial.println(ir_item, HEX);                                  //用16进制显示
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);  // Print "old" raw data
    IrReceiver.resume();                                           // Enable receiving of the next value
  } 
  return ir_item;
}
