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
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // 启动红外接收
}
long current_irdata;

void IR_Read(){
  if (IrReceiver.decode()) {
    last_irdata = current_irdata;
    current_irdata=IrReceiver.decodedIRData.decodedRawData;
    Serial.print("红外遥控：");
    Serial.println(current_irdata,HEX); //用16进制显示的按键值
    IrReceiver.printIRResultShort(&Serial); // Print complete received data in one line
    IrReceiver.resume(); // Enable receiving of the next value
  }
}

long IR_detect(){
  IR_Read();
  switch(current_irdata){
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
      return 0;
  }
}
void IR_DebugInfo(){
  IR_Read();
}