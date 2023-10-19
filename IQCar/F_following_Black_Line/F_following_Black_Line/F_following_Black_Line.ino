// 车上马达 A 的引脚连接
#define MOTOR_RIGTH_PIN_1 6
#define MOTOR_RIGTH_PIN_2 7
#define MOTOR_ENA 11

#define MOTOR_LEFT_PIN_1 4
#define MOTOR_LEFT_PIN_2 5
#define MOTOR_ENB 10

class Moto {
  int pin1;
  int pin2;
  int pin3;
  int lastnum, num = 0;      // 0,1(100),2(175),3(250)
  int laststate, state = 0;  //0 stop, -1 backward, 1, forward
public:
  Moto(int p1, int p2, int p3) {
    pin1 = p1;
    pin2 = p2;
    pin3 = p3;
    // Set the motor control pins to outputs
    pinMode(p1, OUTPUT);
    pinMode(p2, OUTPUT);
  }
  void powerDown() {
    if (num > 0) {
      lastnum = num;
    }
  }
  void powerUp() {
      num += 1;
  }
  void powerOff() {
    lastnum = num;
    num = 0;
  }
  void backward() {
    laststate = state;
    state = -1;
    if (num == 0) {
      lastnum = num;
      num = num + 1;
    }
  }
  void forward() {
    laststate = state;
    state = 1;
    if (num == 0) {
      lastnum = num;
      num = num + 1;
    }
  }
  void speed(int number) {
    num = 200;
  }
  void stop() {
    laststate = state;
    state = 0;
    lastnum = num;
    num = 0;
  }
  void start() {
    laststate = state;
    state = 1;
    lastnum = num;
    num = 1;
  }
  void act() {
    switch (state) {
      case -1:
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, HIGH);
        analogWrite(pin3, 200);
        break;
      case 0:
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, LOW);
        analogWrite(pin3, 0);
        break;
      case 1:
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, LOW);
        analogWrite(pin3, 200);
        break;
    }
  }
  int getNum() {
    return num;
  }
  int getLastNum() {
    return lastnum;
  }
  int getState() {
    return state;
  }
  int getLastState() {
    return laststate;
  }
};
Moto* l = new Moto(MOTOR_LEFT_PIN_1, MOTOR_LEFT_PIN_2, MOTOR_ENB);
Moto* r = new Moto(MOTOR_RIGTH_PIN_1, MOTOR_RIGTH_PIN_2, MOTOR_ENA);
void go() {
  digitalWrite(MOTOR_LEFT_PIN_1,HIGH);
  digitalWrite(MOTOR_LEFT_PIN_2,LOW);
  analogWrite(MOTOR_ENB,200);
  digitalWrite(MOTOR_RIGTH_PIN_1,HIGH);
  digitalWrite(MOTOR_RIGTH_PIN_2,LOW);
  analogWrite(MOTOR_ENA,200);
}

void left() {
  digitalWrite(MOTOR_LEFT_PIN_1,HIGH);
  digitalWrite(MOTOR_LEFT_PIN_2,LOW);
  analogWrite(MOTOR_ENB,200);
  digitalWrite(MOTOR_RIGTH_PIN_1,LOW);
  digitalWrite(MOTOR_RIGTH_PIN_2,LOW);
  analogWrite(MOTOR_ENA,0);
}

void right() {
  digitalWrite(MOTOR_LEFT_PIN_1,LOW);
  digitalWrite(MOTOR_LEFT_PIN_2,LOW);
  analogWrite(MOTOR_ENB,0);
  digitalWrite(MOTOR_RIGTH_PIN_1,HIGH);
  digitalWrite(MOTOR_RIGTH_PIN_2,LOW);
  analogWrite(MOTOR_ENA,200);
}

void stop() {
  digitalWrite(MOTOR_LEFT_PIN_1,LOW);
  digitalWrite(MOTOR_LEFT_PIN_2,LOW);
  analogWrite(MOTOR_ENB,0);
  digitalWrite(MOTOR_RIGTH_PIN_1,LOW);
  digitalWrite(MOTOR_RIGTH_PIN_2,LOW);
  analogWrite(MOTOR_ENA,0);
}

void setup(){
  pinMode(MOTOR_LEFT_PIN_1, OUTPUT);
  pinMode(MOTOR_LEFT_PIN_2, OUTPUT);
  pinMode(MOTOR_RIGTH_PIN_1, OUTPUT);
  pinMode(MOTOR_RIGTH_PIN_2, OUTPUT);
  pinMode(A2, INPUT);
  pinMode(A1, INPUT);
}

void loop(){
  if (digitalRead(A2) == 0 && digitalRead(A1) == 0) {
    stop();

  } else if (digitalRead(A2) == 1 && digitalRead(A1) == 0) {
    right();
  } else if (digitalRead(A2) == 0 && digitalRead(A1) == 1) {
    left();
  } else if (digitalRead(A2) == 1 && digitalRead(A1) == 1) {
    go();
  }

}