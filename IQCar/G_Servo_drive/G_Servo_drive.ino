#include <IRremote.hpp>
#include <Servo.h>
// 车上马达的引脚
#define MOTOR_RIGTH_PIN_1 4
#define MOTOR_RIGTH_PIN_2 5
#define MOTOR_LEFT_PIN_1 6
#define MOTOR_LEFT_PIN_2 7

// 车上舵机的引脚
#define SERVO_PIN 9

// 车上红外摇控接收器的引脚
#define IR_RECEIVE_PIN A0

// 车上左右红外测距接收器的引脚
#define INFRA_DISTANCE_DETECT_LEFT_PIN 11
#define INFRA_DISTANCE_DETECT_RIGHT_PIN 12

// 车上寻迹模块的引脚
#define TRACE_LINE_LEFT_PIN A2
#define TRACE_LINE_RIGHT_PIN A1

// 红色发射器键盘的定义
#define KEY_2 0xE718FF00
#define KEY_5 0xE31CFF00
#define KEY_8 0xAD52FF00
#define KEY_4 0xF708FF00
#define KEY_6 0xA55AFF00
#define KEY_FWD 0xBF40FF00
#define KEY_BWD 0xBB44FF00

Servo myservo;

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
  delay(1);  // 右转一下(毫秒)后，马上直行
  Motor_roll(car.rightWheel, Forward);
}
// 车子左转
void turn_left(Rover car) {
  Motor_roll(car.rightWheel, Forward);
  Motor_roll(car.leftWheel, Stay);
  delay(1);  // 左转一下(毫秒)后，马上直行
  Motor_roll(car.leftWheel, Forward);
}

// 控制舵机转动
unsigned int move_servo(unsigned int pos) {
  if (pos < 1) pos = 1;
  if (pos > 179) pos = 179;
  myservo.write(pos);
  delay(15);
  return pos;
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


unsigned int servo_pos = 0;

void setup() {
  Serial.begin(9600);
  // 绑定舵机
  myservo.attach(SERVO_PIN, 500, 2500);  //修正脉冲宽度
  // 绑定马达
  band_motor(RMotor);
  band_motor(LMotor);
  // 绑定接收器
  IrReceiver.begin(IR_RECEIVE_PIN);
  // 绑定红外测距接收器
  pinMode(INFRA_DISTANCE_DETECT_RIGHT_PIN, INPUT);
  pinMode(INFRA_DISTANCE_DETECT_LEFT_PIN, INPUT);
  // 绑定寻迹感应模块
  pinMode(TRACE_LINE_RIGHT_PIN, INPUT);  // 寻迹右感应引脚
  pinMode(TRACE_LINE_LEFT_PIN, INPUT);   // 寻迹左感应引脚
}
void servo_control() {
  if (IrReceiver.decode()) {
    ir_item = IrReceiver.decodedIRData.decodedRawData;  // 得到接收到的按键值
    IrReceiver.resume();                                // Enable receiving of the next value
  } else {
    if (ir_item == KEY_FWD) {  // 按键 Fast FWD
      servo_pos += 5;
    }
    if (ir_item == KEY_BWD) {  // 按键 Fast BWD
      servo_pos -= 5;
    }
    servo_pos = move_servo(servo_pos);
  }
}

void monitor_Infra_red() {
  int leftPinSignal = digitalRead(TRACE_LINE_LEFT_PIN);
  int rightPinSignal = digitalRead(TRACE_LINE_RIGHT_PIN);

  if (leftPinSignal == LOW && rightPinSignal == LOW) {
    stop(car);
  } else if (leftPinSignal == HIGH && rightPinSignal == LOW) {
    turn_right(car);
  } else if (leftPinSignal == LOW && rightPinSignal == HIGH) {
    turn_left(car);
  } else if (leftPinSignal == HIGH && rightPinSignal == HIGH) {
    move_forward(car);
  }
}

void loop() {
  servo_control();
  monitor_Infra_red();
}
