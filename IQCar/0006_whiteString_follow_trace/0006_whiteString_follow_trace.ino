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
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(A2, INPUT);  // 寻迹左感应引脚
  pinMode(A1, INPUT);  // 寻迹右感应引脚
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
