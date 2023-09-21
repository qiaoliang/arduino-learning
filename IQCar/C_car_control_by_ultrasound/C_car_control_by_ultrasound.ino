#define ULTRASOUND_OUT_PIN 2
#define ULTRASOUND_IN_PIN 3
#define IR_RECEIVE_PIN A0


#define MOTOR_RIGTH_PIN_1 4
#define MOTOR_RIGTH_PIN_2 5
#define MOTOR_LEFT_PIN_1 6
#define MOTOR_LEFT_PIN_2 7

#define KEY_2 0xE718FF00
#define KEY_5 0xE31CFF00
#define KEY_8 0xAD52FF00
#define KEY_4 0xF708FF00
#define KEY_6 0xA55AFF00

enum Direction {
  Forward,
  Stay,
  Backward
};

struct Motor {
  unsigned int pin1;
  unsigned int pin2;
};

struct Motor RMotor = { MOTOR_RIGTH_PIN_1, MOTOR_RIGTH_PIN_2 };
struct Motor LMotor = { MOTOR_LEFT_PIN_1, MOTOR_LEFT_PIN_2 };

void go_forward(){
  Motor_roll(RMotor, Forward);
  Motor_roll(LMotor, Forward);
}

void stop(){
  Motor_roll(RMotor, Stay);
  Motor_roll(LMotor, Stay);
}

void go_backward(){
  Motor_roll(RMotor, Backward);
  Motor_roll(LMotor, Backward);
}

void Motor_roll(Motor &motor, Direction value){
  switch (value) {
    case Forward:
      Serial.println(motor.pin1);
      Serial.println(motor.pin2);
      digitalWrite(motor.pin1, HIGH);
      digitalWrite(motor.pin2, LOW);
      break;
    case Stay: 
      digitalWrite(motor.pin1, LOW);
      digitalWrite(motor.pin2, LOW);
      break;
    case Backward:
      digitalWrite(motor.pin1, LOW);
      digitalWrite(motor.pin2, HIGH);
      break;
  }
}

float last_dist = 0.0;

float checkdistance_By_ultrasound() {
  digitalWrite(ULTRASOUND_OUT_PIN, LOW);
  delayMicroseconds(2);                                            // 在将Trigger 端置为高电平之前,需要确保Echo端已经处于低电平稳定状态，所以等要待2us.
  digitalWrite(ULTRASOUND_OUT_PIN, HIGH);                          // 将Triger 端置高电平,发送超声波,
  delayMicroseconds(10);                                           // 等待10us,让超声波发出
  digitalWrite(ULTRASOUND_OUT_PIN, LOW);                           // 将Trig端置低,结束发送。
  float distance = pulseIn(ULTRASOUND_IN_PIN, HIGH) / 29.033 / 2;  // pulseIn 测量Echo端接受到高电平信号的时间。超声波每厘米需要29.033us。因为是往返时间，所以计算时要除 2。结果的单位是厘米
  delay(10);                                                       // 给传感器一些恢复时间。传感部分的传感器和电路需要一些时间来恢复到准备状态。
  return distance;
}
void band_motor(Motor motor){
  pinMode(motor.pin1, OUTPUT);
  pinMode(motor.pin2, OUTPUT);
}

void setup() {
  Serial.begin(9600);     // 初始化并打开串口通信,设置波特率为9600。波特率指每秒传输的二进制位数。
  // 绑定左右马达
  band_motor(RMotor);
  band_motor(LMotor);

  // 绑定超声波的引脚
  pinMode(ULTRASOUND_OUT_PIN, OUTPUT);
  pinMode(ULTRASOUND_IN_PIN, INPUT);
}

void loop() {
  float dist=checkdistance_By_ultrasound();
  if ( dist< 6) {
    go_backward();
  } else if (dist >= 8 && dist <= 24) {
    go_forward();
  } else if (dist > 24) {
    stop();
  }
}
