#include <IRremote.h>

IRrecv irrecv_A0(A0);
decode_results results_A0;  //红外引脚

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
  irrecv_A0.enableIRIn();  // 红外引脚
}

void loop(){
  if (irrecv_A0.decode(&results_A0)) {
    ir_item=results_A0.value;
    String type="UNKNOWN";
    String typelist[14]={"UNKNOWN", "NEC", "SONY", "RC5", "RC6", "DISH", "SHARP", "PANASONIC", "JVC", "SANYO", "MITSUBISHI", "SAMSUNG", "LG", "WHYNTER"};
    if(results_A0.decode_type>=1&&results_A0.decode_type<=13){
      type=typelist[results_A0.decode_type];
    }
    Serial.print("IR TYPE:"+type+"  ");
    Serial.println(ir_item,HEX); //用16进制显示
    irrecv_A0.resume();
  } else {
    if (ir_item == 0XFF18E7) {    // 按键 2
      go();
    } else if (ir_item == 0xFF4AB5) { // 按键 8
      back();
    } else if (ir_item == 0xFF10EF) { // 按键 6
      right();
    } else if (ir_item == 0xFF5AA5) { // 按键 4
      left();
    } else if (ir_item == 0xFF38C7) { // 按键 6
      stop();
    }
  }

}
