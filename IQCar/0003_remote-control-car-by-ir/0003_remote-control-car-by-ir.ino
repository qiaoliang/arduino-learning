#include <IRremote.hpp>

#define IR_RECEIVE_PIN A0   //红外引脚号 

long ir_item;

void rightWheelForward(){
  digitalWrite(4,HIGH);
  digitalWrite(5,LOW);
}
void rightWheelBackward(){
  digitalWrite(4,LOW);
  digitalWrite(5,HIGH);
}

void rightWheelStop(){
  digitalWrite(4,LOW);
  digitalWrite(5,LOW);
}

void leftWheelForward(){
  digitalWrite(6,HIGH);
  digitalWrite(7,LOW);
}
void leftWheelBackward(){
  digitalWrite(6,LOW);
  digitalWrite(7,HIGH);
}
void leftWheelStop(){
  digitalWrite(6,LOW);
  digitalWrite(7,LOW);
}

void back() {
  leftWheelBackward();
  rightWheelBackward();
}

void go() {
  leftWheelForward();
  rightWheelForward();
}

void left() {
  rightWheelForward();
  leftWheelStop();
}

void right() {
  leftWheelForward();
  rightWheelStop();
}

void stop() {
  rightWheelStop();
  leftWheelStop();
}

void setup(){
  Serial.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // 启动红外接收
}

void move(long ir_item){
    if (ir_item == 0xE718FF00) {    // 按键 2
      go();
    } else if (ir_item == 0xAD52FF00) { // 按键 8
      back();
    } else if (ir_item == 0xA55AFF00) { // 按键 6
      right();
    } else if (ir_item == 0xF708FF00) { // 按键 4
      left();
    } else if (ir_item == 0xE31CFF00) { // 按键 5
      stop();
    }
}
void loginfo(){
    Serial.println(ir_item,HEX); //用16进制显示的按键值
    IrReceiver.printIRResultShort(&Serial); // Print complete received data in one line
}
void loop(){
  if (IrReceiver.decode()) {
    ir_item=IrReceiver.decodedIRData.decodedRawData;
    loginfo();
    IrReceiver.resume(); // Enable receiving of the next value
  } else {
    move(ir_item);
  }

}
