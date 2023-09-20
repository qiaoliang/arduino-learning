#include <IRremote.hpp>

#define IR_RECEIVE_PIN A0

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

void right() {
  rightWheelForward();
  leftWheelStop();
}

void left() {
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
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

void loop(){
  if (IrReceiver.decode()) {
    ir_item=IrReceiver.decodedIRData.decodedRawData;
    // int ir_protocol = IrReceiver.decodedIRData.protocol;  not used at all.
    Serial.println(ir_item,HEX); //用16进制显示
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX); // Print "old" raw data
    // Print out complete received data in one line
    // and  the output format should be like  'Protocol=NEC Address=0x0 Command=0x5A Raw-Data=0xA55AFF00 32 bits LSB first'
    // Sometimes, the portocol is 'UNKNOWN', which just is noise signal.
    IrReceiver.printIRResultShort(&Serial); 
    IrReceiver.resume(); // Enable receiving of the next value
  } else {
    if (ir_item == 0xE718FF00/*0XFF18E7*/) {    // 按键 2
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

}
