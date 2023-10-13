// 本程序使用 L298N驱动板上的 ENA 和 ENB 两个引脚，通过 PWM 信号来控制小车的行进速度。
// 车上马达 A 的引脚连接
#define MOTOR_RIGTH_PIN_1 4
#define MOTOR_RIGTH_PIN_2 5
#define MOTOR_ENA         6

#define MOTOR_LEFT_PIN_1  7
#define MOTOR_LEFT_PIN_2  8
#define MOTOR_ENB        9


struct Motor {
  unsigned int pin1;  // 引脚 1
  unsigned int pin2;  // 引脚 2
  unsigned int enable;  // 引脚 enable
};

void setup() {
  Serial.begin(9600);

    // Set all the motor control pins to outputs
    pinMode(MOTOR_ENA, OUTPUT);
    pinMode(MOTOR_ENB, OUTPUT);
    pinMode(MOTOR_RIGTH_PIN_1, OUTPUT);
    pinMode(MOTOR_RIGTH_PIN_2, OUTPUT);
    pinMode(MOTOR_LEFT_PIN_1, OUTPUT);
    pinMode(MOTOR_LEFT_PIN_2, OUTPUT);
    
    // Turn off motors - Initial state
    digitalWrite(MOTOR_RIGTH_PIN_1, LOW);
    digitalWrite(MOTOR_RIGTH_PIN_2, LOW);
    digitalWrite(MOTOR_LEFT_PIN_1, LOW);
    digitalWrite(MOTOR_LEFT_PIN_2, LOW);
}

void loop() {
    directionControl();
    delay(1000);
    speedControl();
    delay(1000);
}

// This function lets you control spinning direction of motors
void directionControl() {
    // Set motors to maximum speed
    // For PWM maximum possible values are 0 to 255
    analogWrite(MOTOR_ENA, 255);
    analogWrite(MOTOR_ENB, 255);

    // Turn on motor A & B
    digitalWrite(MOTOR_RIGTH_PIN_1, HIGH);
    digitalWrite(MOTOR_RIGTH_PIN_2, LOW);
    digitalWrite(MOTOR_LEFT_PIN_1, HIGH);
    digitalWrite(MOTOR_LEFT_PIN_2, LOW);
    delay(2000);
    
    // Now change motor directions
    digitalWrite(MOTOR_RIGTH_PIN_1, LOW);
    digitalWrite(MOTOR_RIGTH_PIN_2, HIGH);
    digitalWrite(MOTOR_LEFT_PIN_1, LOW);
    digitalWrite(MOTOR_LEFT_PIN_2, HIGH);
    delay(2000);
    
    // Turn off motors
    digitalWrite(MOTOR_RIGTH_PIN_1, LOW);
    digitalWrite(MOTOR_RIGTH_PIN_2, LOW);
    digitalWrite(MOTOR_LEFT_PIN_1, LOW);
    digitalWrite(MOTOR_LEFT_PIN_2, LOW);
}

// This function lets you control speed of the motors
void speedControl() {
    // Turn on motors
    digitalWrite(MOTOR_RIGTH_PIN_1, LOW);
    digitalWrite(MOTOR_RIGTH_PIN_2, HIGH);
    digitalWrite(MOTOR_LEFT_PIN_1, LOW);
    digitalWrite(MOTOR_LEFT_PIN_2, HIGH);
    
    // Accelerate from zero to maximum speed
    for (int i = 0; i < 256; i++) {
        analogWrite(MOTOR_ENA, i);
        analogWrite(MOTOR_ENB, i);
        delay(20);
    }
    
    // Decelerate from maximum speed to zero
    for (int i = 255; i >= 0; --i) {
        analogWrite(MOTOR_ENA, i);
        analogWrite(MOTOR_ENB, i);
        delay(20);
    }
    
    // Now turn off motors
    digitalWrite(MOTOR_RIGTH_PIN_1, LOW);
    digitalWrite(MOTOR_RIGTH_PIN_2, LOW);
    digitalWrite(MOTOR_LEFT_PIN_1, LOW);
    digitalWrite(MOTOR_LEFT_PIN_2, LOW);
}
