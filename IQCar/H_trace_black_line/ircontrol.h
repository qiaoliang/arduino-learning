#include <IRremote.hpp>

#define IR_RECEIVE_PIN A4  // 车上红外摇控接收器的引脚

// 手中红外遥控键盘的键值
#define KEY_5 0xE31CFF00    // 向后
#define KEY_2 0xE718FF00    //向前   
#define KEY_8 0xAD52FF00    // 向后
#define KEY_4 0xF708FF00    // 左转
#define KEY_6 0xA55AFF00    // 右转
#define KEY_STOP 0xBB44FF00 // 停止
#define KEY_ACC 0xBC43FF00  // 加速
#define KEY_DAC 0xBF40FF00  // 减速

long ir_item;
extern Rover* rover;          // 已经在主文件中声明并初始化了
/** 
 * 初始化A4引脚中断
 */
void IrReceiver_ISR_init(){
  IrReceiver.begin(IR_RECEIVE_PIN); // 绑定遥控器接收模块
  PCICR |=0x02;  // Enable PCIE Port C，端口等参见  https://blog.csdn.net/acktomas/article/details/128230329
  PCMSK1 |=0x10;  // 引脚 A4
}

/**
 * irRemote中断的处理函数
 */
void IrReciver_Routine(){
  if (IrReceiver.decode()) {
    Serial.println("~~~~~~~~~~~~~~~~~~");
    ir_item = IrReceiver.decodedIRData.decodedRawData;
    Serial.println(KEY_STOP, HEX); 
    if(ir_item == KEY_STOP||ir_item == KEY_5) {
      Serial.println(KEY_STOP, HEX);                                  //用16进制显示
      rover->stop();
    }
    if(ir_item == KEY_2) {
      Serial.println(KEY_2, HEX);                                  //用16进制显示
      rover->forward();
    }
    if(ir_item == KEY_8) {
      Serial.println(KEY_8, HEX);
      rover->backward();
    }
    if(ir_item == KEY_4) {
      Serial.println(KEY_4, HEX);                                  //用16进制显示
      rover->left();
    }
    if(ir_item == KEY_6) {
      Serial.println(KEY_6, HEX);                                  //用16进制显示
      rover->right();
    }
    if(ir_item == KEY_ACC){
       rover->powerUp();
    }
    if(ir_item == KEY_DAC){
       rover->powerDown();
    }
    else{
      Serial.print(">>>>  ");
      Serial.println(ir_item, HEX);                                  //用16进制显示
    }
    IrReceiver.resume();                                           // Enable receiving of the next value
  }
}