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
  pinMode(11, INPUT);
  pinMode(12, INPUT);
}

void loop(){
  if (digitalRead(11) == 0 && digitalRead(12) == 0) {
    stop();
    delay(200);
    back();
    delay(300);
    left();
    delay(300);

  } else if (digitalRead(11) == 1 && digitalRead(12) == 0) {
    stop();
    delay(200);
    back();
    delay(300);
    left();
    delay(300);
  } else if (digitalRead(11) == 0 && digitalRead(12) == 1) {
    stop();
    delay(200);
    back();
    delay(300);
    right();
    delay(300);
  } else if (digitalRead(11) == 1 && digitalRead(12) == 1) {
    go();
  }

}
