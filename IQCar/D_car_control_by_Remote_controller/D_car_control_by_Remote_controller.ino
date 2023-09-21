#include <IRremote.hpp>

#define MOTOR_RIGTH_PIN_1 4
#define MOTOR_RIGTH_PIN_2 5
#define MOTOR_LEFT_PIN_1 6
#define MOTOR_LEFT_PIN_2 7

// 车上红外接收器的引脚
#define IR_RECEIVE_PIN A0

// 红色发身器键盘的定义
#define KEY_2 0xE718FF00
#define KEY_5 0xE31CFF00
#define KEY_8 0xAD52FF00
#define KEY_4 0xF708FF00
#define KEY_6 0xA55AFF00

enum Direction {
  Forward,  // 向前转
  Stay,     // 静止不动
  Backward  // 向后转
};

struct Motor {
  unsigned int pin1;  // 引脚 1
  unsigned int pin2;  // 引脚 2
};

struct Rover {
  struct Motor *rightWheel;
  struct Motor *leftWheel;
};

void Motor_roll(Motor *motor, Direction value) {
  switch (value) {
    case Forward:  // 向前转
      digitalWrite(motor->pin1, HIGH);
      digitalWrite(motor->pin2, LOW);
      break;
    case Stay:  // 静止不动
      digitalWrite(motor->pin1, LOW);
      digitalWrite(motor->pin2, LOW);
      break;
    case Backward:  // 向后转
      digitalWrite(motor->pin1, LOW);
      digitalWrite(motor->pin2, HIGH);
      break;
  }
}

// 车子向前
void move_forward(Rover car) {
  Motor_roll(car.rightWheel, Forward);
  Motor_roll(car.leftWheel, Forward);
}
// 车子停止
void stop(Rover car) {
  Motor_roll(car.rightWheel, Stay);
  Motor_roll(car.leftWheel, Stay);
}
// 车子向后
void move_backward(Rover car) {
  Motor_roll(car.rightWheel, Backward);
  Motor_roll(car.leftWheel, Backward);
}
// 车子右转
void turn_right(Rover car) {
  Motor_roll(car.rightWheel, Stay);
  Motor_roll(car.leftWheel, Forward);
}
// 车子右转
void turn_left(Rover car) {
  Motor_roll(car.rightWheel, Forward);
  Motor_roll(car.leftWheel, Stay);
}

long ir_item;
// 绑定马达引脚
void band_motor(Motor motor) {
  pinMode(motor.pin1, OUTPUT);
  pinMode(motor.pin2, OUTPUT);
}
// 初始化车上的两个马达
struct Motor RMotor = { MOTOR_RIGTH_PIN_1, MOTOR_RIGTH_PIN_2 };
struct Motor LMotor = { MOTOR_LEFT_PIN_1, MOTOR_LEFT_PIN_2 };

// 初始化小车
struct Rover car = { &RMotor, &LMotor };

void setup() {
  Serial.begin(9600);
  band_motor(RMotor);
  band_motor(LMotor);
  // 绑定接收器
  IrReceiver.begin(IR_RECEIVE_PIN);
}

void loop() {
  if (IrReceiver.decode()) {
    ir_item = IrReceiver.decodedIRData.decodedRawData;             // 得到接收到的按键值
    Serial.println(ir_item, HEX);                                  //用16进制显示
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);  // Print "old" raw data
    IrReceiver.resume();                                           // Enable receiving of the next value
  } else {
    if (ir_item == KEY_2) {  // 按键 2
      move_forward(car);
    } else if (ir_item == KEY_8) {  // 按键 8
      move_backward(car);
    } else if (ir_item == KEY_6) {  // 按键 6
      turn_right(car);
    } else if (ir_item == KEY_4) {  // 按键 4
      turn_left(car);
    } else if (ir_item == KEY_5) {  // 按键 5
      stop(car);
    }
  }
}
