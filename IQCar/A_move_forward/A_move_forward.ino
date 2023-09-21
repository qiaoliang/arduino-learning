#define RIGHT_WHEEL_1 4
#define RIGHT_WHEEL_2 5
#define LEFT_WHEEL_1 6
#define LEFT_WHEEL_2 7

void go(){
  digitalWrite(4,HIGH);
  digitalWrite(5,LOW);
  digitalWrite(6,HIGH);
  digitalWrite(7,LOW);
}


void setup(){
  pinMode(RIGHT_WHEEL_1, OUTPUT);
  pinMode(RIGHT_WHEEL_2, OUTPUT);
  pinMode(6LEFT_WHEEL_1, OUTPUT);
  pinMode(LEFT_WHEEL_2, OUTPUT);
}

void loop(){
  go();
}
