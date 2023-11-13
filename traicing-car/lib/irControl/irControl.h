#ifndef __IRCONTROL_H__
#define __IRCONTROL_H__

#include <Arduino.h>
#include <IRremote.hpp>

// 车上红外摇控接收器的引脚
#define IR_RECEIVE_PIN A5

// 红色发射器键盘的定义
#define KEY_POWER 0x45
#define KEY_MODE 0x46
#define KEY_MUTE 0x47
#define KEY_START_STOP 0x44
#define KEY_PREVIOUS 0x40
#define KEY_NEXT 0x43
#define KEY_EQ 0x07
#define KEY_VOL_MINUS 0x15
#define KEY_VOL_ADD 0x09
#define KEY_0 0x16
#define KEY_RPT 0x19
#define KEY_USD 0x0D
#define KEY_1 0x0C
#define KEY_2 0x18
#define KEY_3 0x5E
#define KEY_4 0x08
#define KEY_5 0x1C
#define KEY_6 0x5A
#define KEY_7 0x42
#define KEY_8 0x52
#define KEY_9 0x4A

#include <IRremote.hpp>
uint8_t last_key, current_key;

char getKey(uint8_t value){
  char ret;
  switch (value)
  {
  case KEY_2:
    ret = '2';
    break;
  case KEY_4:
    ret = '4';
    break;
  case KEY_6:
    ret = '6';
    break;
  case KEY_8:
    ret = '8';
    break;
  case KEY_5:
    ret = '5';
    break;
  default:
    ret ='?';
    break;
  }
  return ret;

}
void IR_Init()
{
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // 启动红外接收
}

uint8_t IR_detect()
{
    uint8_t ret='m';
    if (IrReceiver.decode())
    {
        last_key = current_key;
        current_key = IrReceiver.decodedIRData.command;
        IrReceiver.printIRResultShort(&Serial); // Print complete received data in one line
        IrReceiver.resume();                    // Enable receiving of the next value
        ret = getKey(current_key);
    }
    return ret;
}

void IR_DebugInfo()
{
    IR_detect();
    Serial.print("红外遥控：");
    Serial.print(current_key, HEX); // 用16进制显示的按键值
    Serial.print(" --> ");
    Serial.println(getKey(current_key));
    Serial.println(current_key, HEX); // 用16进制显示的按键值
}
#endif