#include <IRremote.hpp>

#define IR_RECEIVE_PIN A4  // 车上红外摇控接收器的引脚

#define KEY_2 0xE718FF00   // 手中红外遥控键盘的键值
#define KEY_5 0xE31CFF00
#define KEY_8 0xAD52FF00
#define KEY_4 0xF708FF00
#define KEY_6 0xA55AFF00
#define KEY_STOP 0xBB44FF00

long ir_item;
extern Rover* rover;          // 在主文件中已经声明并初始化了
void IrReceiver_ISR_init(){   // 初始化A4引脚中断
  IrReceiver.begin(IR_RECEIVE_PIN); // 绑定遥控器接收模块
  PCICR |=0x02;  // Enable PCIE Port C，端口等参见  https://blog.csdn.net/acktomas/article/details/128230329
  PCMSK1 |=0x10;  // 引脚 A4
}

void IrReciver_exec(){
  if (IrReceiver.decode()) {
    ir_item = IrReceiver.decodedIRData.decodedRawData;             // 得到接收到的按键值
    Serial.println(ir_item, HEX);                                  //用16进制显示
    if(ir_item == KEY_STOP) {
      Serial.println("KEY_STOP");
      rover->stop();
    }
    IrReceiver.resume();                                           // Enable receiving of the next value
  }
}